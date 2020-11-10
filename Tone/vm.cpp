#include "vm.h"
#include "nativeobject.h"
#include "closureobject.h"
#include "instanceobject.h"
#include "boundmethodobject.h"

#include <iostream>
#include <stdarg.h> // Q: is this needed (for runtimeError())?

//#define DEBUG_TRACE_EXECUTION

#define AS_FUNCTION(object)  ((FunctionObject*)(object)) // Q: is there a better way of doing this than a macro?
#define AS_CLOSURE(object)  ((ClosureObject*)(object)) // Q: ditto
#define AS_NATIVE(object)        (((NativeObject*)(object))->getFunction()) // Q: ditto
#define AS_INSTANCE(object)        ((InstanceObject*)(object)) // Q: ditto
#define AS_CLASS(object)        ((ClassObject*)(object)) // Q: ditto
#define AS_BOUND_METHOD(object)        ((BoundMethodObject*)(object)) // Q: ditto

VM::VM() {
  objects = nullptr;
  openUpvalues = nullptr;
  callFrameCount = 0;
  initString = copyString("init");
  //defineNative("clock", this->clockNative);
}

// Q: is this needed?
InterpretResult VM::interpret() {
  return run();
}

InterpretResult VM::run() {
  CallFrame* frame = &callFrames[callFrameCount - 1]; // Q: why a pointer?

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << "          ";
    stack.print();
    std::cout << '\n';
    frame->closure->getFunction()->getChunk()->disassembleInstruction(frame->functionProgramCounter);
    //chunk.disassembleInstruction(programCounter);
#endif // DEBUG_TRACE_EXECUTION
    uint8_t instruction;
    switch (instruction = readByte(frame)) {
      case OP_CONSTANT: {
        Value constant = readConstant(frame);
        stack.push(constant);
        break;
      }
      // Q: how can I write this so I don't have to pass in 0?
      case OP_NULL: stack.push(Value{ VAL_NULL, 0 }); break;
      case OP_TRUE: stack.push(Value{ true }); break;
      case OP_FALSE: stack.push(Value{ false }); break;
      case OP_POP: stack.pop(); break;
      case OP_GET_LOCAL: {
        uint8_t slot = readByte(frame);
        stack.push(frame->slots[slot]);
        break;
      }

      case OP_SET_LOCAL: {
        uint8_t slot = readByte(frame);
        frame->slots[slot] = stack.peek(0);
        //stack.set(slot, stack.peek(0));
        break;
      }

      case OP_GET_GLOBAL: {
        StringObject* name = readString(frame);
        Value value;
        std::unordered_map<std::string, Value>::iterator it = globals.find(name->getChars());
        if (it == globals.end()) {
          runtimeError("Undefined variable '%s'.", name->getChars().c_str());
          return INTERPRET_RUNTIME_ERROR;
        }
        value = it->second;
        stack.push(value);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        StringObject* name = readString(frame);
        globals[name->getChars()] = stack.peek(0);
        stack.pop();
        break;
      }

      case OP_SET_GLOBAL: {
        StringObject* name = readString(frame);
        std::unordered_map<std::string, Value>::iterator it = globals.find(name->getChars());
        if (it == globals.end()) {
          // Implicit variable declaration not allowed
          // Todo: change getChars and chars to getString and string
          runtimeError("Undefined variable '%s'.", name->getChars().c_str());
          return INTERPRET_RUNTIME_ERROR;
        } else {
          it->second = stack.peek(0);
        }

        break;
      }

      case OP_GET_UPVALUE: {
        uint8_t slot = readByte(frame);
        stack.push(*frame->closure->getUpvalue(slot)->getLocation());
        break;
      }

      case OP_SET_UPVALUE: {
        uint8_t slot = readByte(frame);
        frame->closure->getUpvalue(slot)->setLocationValue(stack.peek(0));
        break;
      }

      case OP_GET_PROPERTY: {
        if (!stack.peek(0).isInstance()) {
          runtimeError("Only instances have properties.");
          return INTERPRET_RUNTIME_ERROR;
        }

        InstanceObject* instance = AS_INSTANCE(stack.peek(0).asObject());
        StringObject* name = readString(frame);

        Value value;

        if (instance->hasField(name->getChars())) {
          value = instance->getField(name->getChars()); // Q: should we be getting a pointer back?
          stack.pop();
          stack.push(value);
          break;
        }

        if (!bindMethod(instance->getKlass(), name)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }

      case OP_SET_PROPERTY: {
        if (!stack.peek(1).isInstance()) {
          runtimeError("Only instances have fields.");
          return INTERPRET_RUNTIME_ERROR;
        }

        InstanceObject* instance = AS_INSTANCE(stack.peek(1).asObject());
        instance->setField(readString(frame)->getChars(), stack.peek(0));
        Value value = stack.pop();
        stack.pop();
        stack.push(value);
        break;
      }

      case OP_GET_SUPER: {
        StringObject* name = readString(frame);
        ClassObject* superclass = AS_CLASS(stack.pop().asObject());
        if (!bindMethod(superclass, name)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }

      case OP_EQUAL: {
        Value b = stack.pop();
        Value a = stack.pop();
        stack.push(Value{ valuesEqual(a, b) });
        break;
      }
      case OP_GREATER:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        greaterThan();
        break;
      case OP_LESS:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        lessThan();
        break;
      case OP_ADD:
        if (stack.peek(0).isString() && stack.peek(1).isString()) {
          StringObject* b = stack.pop().asString();
          StringObject* a = stack.pop().asString();

          // Q: how do I ensure that memory isn't leaked here and around here?
          StringObject* result = new StringObject((*a).getChars() + (*b).getChars());
          stack.push(Value{ result });
        } else if (stack.peek(0).isNumber() && stack.peek(1).isNumber()) {
          add();
        } else {
          runtimeError("Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      case OP_SUBTRACT:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        subtract();
        break;
      case OP_DIVIDE:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        divide();
        break;
      case OP_MULTIPLY:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        multiply();
        break;
      case OP_NOT:
        stack.push(Value{ stack.pop().isFalsey() });
        break;
      case OP_NEGATE: {
        if (!stack.peek(0).isNumber()) {
          runtimeError("Operand must be a number."); // Q: should this be handled by ErrorReporter?
          return INTERPRET_RUNTIME_ERROR;
        }

        double negatedValue = -(stack.pop().asNumber());
        stack.push(Value{ VAL_NUMBER, negatedValue });
        break;
      }
      case OP_PRINT: {
        std::cout << stack.pop() << '\n';
        break;
      }

      case OP_JUMP: {
        uint16_t offset = readShort(frame);
        frame->functionProgramCounter += offset;
        //programCounter += offset;
        break;
      }

      case OP_JUMP_IF_FALSE: {
        uint16_t offset = readShort(frame);
        if (stack.peek(0).isFalsey()) frame->functionProgramCounter += offset;
        //if (stack.peek(0).isFalsey()) programCounter += offset;
        break;
      }

      case OP_LOOP: {
        uint16_t offset = readShort(frame);
        frame->functionProgramCounter -= offset;
        //programCounter -= offset;
        break;
      }

      case OP_CALL: {
        int argCount = readByte(frame); // Q: is a cast to int needed/preferable here?
        if (!callValue(stack.peek(argCount), argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &(callFrames[callFrameCount - 1]); // Q: parentheses needed?
        break;
      }

      case OP_INVOKE: {
        StringObject* method = readString(frame);
        int argCount = readByte(frame);
        if (!invoke(method, argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &(callFrames[callFrameCount - 1]);
        break;
      }

      case OP_SUPER_INVOKE: {
        StringObject* method = readString(frame);
        int argCount = readByte(frame);
        ClassObject* superclass = AS_CLASS(stack.pop().asObject());
        if (!invokeFromClass(superclass, method, argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &(callFrames[callFrameCount - 1]);
        break;
      }

      case OP_CLOSURE: {
        FunctionObject* function = AS_FUNCTION(readConstant(frame).asObject());
        ClosureObject* closure = new ClosureObject(function); // Q: how do I avoid memory leaks here?
        stack.push(Value{ closure });

        for (int i = 0; i < closure->getFunction()->getUpvalueCount(); i++) {
          uint8_t isLocal = readByte(frame);
          uint8_t index = readByte(frame);
          if (isLocal) {
            closure->setUpvalue(i, captureUpvalue(frame->slots + index));
          } else {
            closure->setUpvalue(i, frame->closure->getUpvalue(index));
          }
        }

        break;
      }

      case OP_CLOSE_UPVALUE:
        closeUpvalues(stack.getTop() - 1);
        stack.pop();
        break;

      case OP_RETURN: {
        Value result = stack.pop();

        closeUpvalues(frame->slots);

        callFrameCount--;
        if (callFrameCount == 0) {
          stack.pop();
          return INTERPRET_OK;
        }

        // Q: is pointer assignment what I actually want to do here?
        // (See the implementation of setTop)
        stack.setTop(frame->slots);
        stack.push(result);

        frame = &(callFrames[callFrameCount - 1]);
        break;
      }

      case OP_CLASS: {
        ClassObject* classObject = new ClassObject{ readString(frame) }; // Q: how to avoid memory leaks?
        stack.push(Value{ classObject });
        break;
      }

      case OP_INHERIT: {
        Value superclass = stack.peek(1);
        if (!superclass.isClass()) {
          runtimeError("Superclass must be a class.");
          return INTERPRET_RUNTIME_ERROR;
        }

        ClassObject* subclass = AS_CLASS(stack.peek(0).asObject());
        ClassObject* superclassAsClass = AS_CLASS(superclass.asObject());
        std::unordered_map<std::string, Value>* superclassMethods = superclassAsClass->getMethods();
        subclass->getMethods()->insert(superclassMethods->begin(), superclassMethods->end());
        stack.pop(); // Subclass.
        break;
      }

      case OP_METHOD: {
        defineMethod(readString(frame));
        break;
      }
    }
  }
}

uint8_t VM::readByte(CallFrame* frame) {
  return frame->closure->getFunction()->getChunk()->getBytecode().at(frame->functionProgramCounter++);

  //return chunk.getBytecode().at(programCounter++);
}

uint16_t VM::readShort(CallFrame* frame) {
  frame->functionProgramCounter += 2;
  uint8_t firstPart = frame->closure->getFunction()->getChunk()->getBytecode().at(frame->functionProgramCounter-2) << 8;
  uint8_t secondPart = frame->closure->getFunction()->getChunk()->getBytecode().at(frame->functionProgramCounter-1);

//  programCounter += 2;
//  uint8_t firstPart = chunk.getBytecode().at(programCounter-2) << 8;
//  uint8_t secondPart = chunk.getBytecode().at(programCounter-1);

  return firstPart | secondPart;
}

Value VM::readConstant(CallFrame* frame) {
  return frame->closure->getFunction()->getChunk()->getConstants().at(readByte(frame));

  //return chunk.getConstants().at(readByte());
}

StringObject* VM::readString(CallFrame* frame) {
  return readConstant(frame).asString();
}

void VM::add() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ VAL_NUMBER, a + b });
}

void VM::subtract() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ VAL_NUMBER, a - b });
}

void VM::divide() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ VAL_NUMBER, a / b });
}

void VM::multiply() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ VAL_NUMBER, a * b });
}

void VM::greaterThan() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ a > b });
}

void VM::lessThan() {
  double b = stack.pop().asNumber();
  double a = stack.pop().asNumber();
  stack.push(Value{ a < b });
}

void VM::runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = callFrameCount - 1; i >= 0; i--) {
    CallFrame* frame = &(callFrames[i]);
    FunctionObject* function = frame->closure->getFunction();
    int instruction = frame->functionProgramCounter - 1;
    std::cerr << "[line " << function->getChunk()->getLine(instruction) << "] in ";
    if (function->getName() == NULL) { // Q: could this be == nullptr?
      std::cerr << "script\n";
    } else {
      std::cerr << function->getName()->getChars() << '\n';
    }
  }

  stack.reset();
  callFrameCount = 0;
  openUpvalues = nullptr;
}

void VM::defineMethod(StringObject* name) {
  Value method = stack.peek(0);
  ClassObject* klass = AS_CLASS(stack.peek(1).asObject());
  klass->setMethod(name->getChars(), method);
  stack.pop();
}

bool VM::bindMethod(ClassObject* klass, StringObject* name) {
  Value method;

  if (!klass->findMethod(name->getChars())) {
    runtimeError("Undefined property '%s'.", name->getChars().c_str());
    return false;
  }

  method = klass->getMethod(name->getChars());

  // Q: avoid memory leaks?
  BoundMethodObject* bound = new BoundMethodObject(stack.peek(0), AS_CLOSURE(method.asObject()));
  stack.pop();
  stack.push(Value{ bound });
  return true;
}

bool VM::invoke(StringObject* name, int argCount) {
  Value receiver = stack.peek(argCount);

  if (!receiver.isInstance()) {
    runtimeError("Only instances have methods.");
    return false;
  }

  InstanceObject* instance = AS_INSTANCE(receiver.asObject());

  Value value;

  if (instance->hasField(name->getChars())) {
    value = instance->getField(name->getChars());
    Value* valueToSet = stack.getTop() - argCount - 1;
    *valueToSet = value;
    return callValue(value, argCount);
  }

  return invokeFromClass(instance->getKlass(), name, argCount);
}

bool VM::invokeFromClass(ClassObject* klass, StringObject* name, int argCount) {
  Value method;

  if (klass->findMethod(name->getChars())) {
    method = klass->getMethod(name->getChars());
  } else {
    runtimeError("Undefined property '%s'.", name->getChars().c_str());
    return false;
  }

  return call(AS_CLOSURE(method.asObject()), argCount);
}

bool VM::valuesEqual(Value a, Value b) {
  if (a.getType() != b.getType()) return false;

  switch (a.getType()) {
    case VAL_BOOL:   return a.asBool() == b.asBool();
    case VAL_NULL:   return true;
    case VAL_NUMBER: return a.asNumber() == b.asNumber();
    case VAL_OBJECT: {
      return a.asObject() == b.asObject();
    }
    default:
      return false;
  }
}

Stack* VM::getStack() {
  return &stack;
}

bool VM::callValue(Value callee, int argCount) {
  if (callee.isObject()) {
    switch (callee.getObjectType()) {
      case OBJECT_BOUND_METHOD: {
        BoundMethodObject* bound = AS_BOUND_METHOD(callee.asObject());
        Value* valueToSet = stack.getTop() - argCount - 1;
        *valueToSet = bound->getReceiver();
        return call(bound->getMethod(), argCount);
      }

      case OBJECT_CLASS: {
        ClassObject* klass = AS_CLASS(callee.asObject());
        InstanceObject* instance = new InstanceObject{ klass };
        Value value = Value{ instance };
        Value* valueToSet = stack.getTop() - argCount - 1;
        *valueToSet = value;

        Value initializer;

        if (klass->findMethod(initString->getChars())) {
          initializer = klass->getMethod(initString->getChars());
          return call(AS_CLOSURE(initializer.asObject()), argCount);
        } else if (argCount != 0) {
          runtimeError("Expected 0 arguments but got %d.", argCount);
          return false;
        }

        return true;
      }

      case OBJECT_CLOSURE: {
        Object* calleeAsObject = callee.asObject();
        return call(AS_CLOSURE(calleeAsObject), argCount);
      }

      case OBJECT_NATIVE: {
        Object* calleeAsObject = callee.asObject();
        Value (*native)(int argCount, Value* args) = AS_NATIVE(calleeAsObject);
        Value result = native(argCount, stack.getTop() - argCount);
        stack.setTop(stack.getTop() - argCount + 1);
        stack.push(result);
        return true;
      }

      default:
        // Non-callable object type.
        break;
    }
  }

  runtimeError("Can only call functions and classes.");
  return false;
}

bool VM::call(ClosureObject* closure, int argCount) {
  if (argCount != closure->getFunction()->getArity()) {
    runtimeError("Expected %d arguments but got %d.",
        closure->getFunction()->getArity(), argCount);
    return false;
  }

  if (callFrameCount == callFramesMax) {
    runtimeError("Stack overflow.");
    return false;
  }

  CallFrame* frame = &(callFrames[callFrameCount++]);
  frame->closure = closure;
  frame->functionProgramCounter = 0;

  frame->slots = stack.getTop() - argCount - 1;
  return true;
}

UpvalueObject* VM::captureUpvalue(Value* local) {
  UpvalueObject* previousUpvalue = nullptr;
  UpvalueObject* upvalue = openUpvalues;

  while (upvalue != NULL && upvalue->getLocation() > local) {
    previousUpvalue = upvalue;
    upvalue = upvalue->getNext();
  }

  if (upvalue != NULL && upvalue->getLocation() == local) return upvalue;

  UpvalueObject* createdUpvalue = new UpvalueObject{ local }; // Q: how to avoid memory leaks?

  createdUpvalue->setNext(upvalue);

  if (previousUpvalue == NULL) {
    openUpvalues = createdUpvalue;
  } else {
    previousUpvalue->setNext(createdUpvalue);
  }

  return createdUpvalue;
}

void VM::closeUpvalues(Value* last) {
  while (openUpvalues != NULL &&
         openUpvalues->getLocation() >= last) {
    UpvalueObject* upvalue = openUpvalues;
    upvalue->setClosed(*(upvalue->getLocation()));
    upvalue->setLocation(upvalue->getClosed());
    openUpvalues = upvalue->getNext();
  }
}

CallFrame* VM::getCallFrames() {
  return callFrames;
}

int VM::getCallFrameCount() {
  return callFrameCount;
}

void VM::incrementCallFrameCount() {
  callFrameCount++;
}

//void VM::defineNative(const std::string &name, nativeFunctionPointer function) {
//  stack.push(Value{ copyString(name) });
//  NativeObject* newNative = new NativeObject(function); // Q: how can I make sure this dynamically allocated memory gets deleted?
//  stack.push(Value{ newNative });
//  globals.insert(std::make_pair(stack.at(0).asString()->getChars(), stack.at(1)));
//  stack.pop();
//  stack.pop();
//}

StringObject* VM::copyString(const std::string &name) {
  // Q: how can I make sure that stringObject gets deleted and memory gets freed at the right time?

  std::unordered_map<std::string, Value>::iterator it = strings.find(name);
  if (it == strings.end()) {
    StringObject* stringObject = new StringObject(name);
    strings.insert(std::make_pair(name, Value{ stringObject }));

    return stringObject;
  }

  return (it->second).asString(); // Q: are parentheses necessary?
}

//Value VM::clockNative(int argCount, Value* args) {
//  return Val{ (double)clock() / CLOCKS_PER_SEC };
//}
