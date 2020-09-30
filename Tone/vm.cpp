#include <iostream>
#include <stdarg.h> // Q: is this needed (for runtimeError())?

#include "vm.h"

#define DEBUG_TRACE_EXECUTION

VM::VM() {
  chunk = Chunk();
  objects = nullptr;
}

VM::VM(Chunk chunk, Object* objects) : chunk{ chunk }, objects{ objects } {}

// Q: is this needed?
InterpretResult VM::interpret() {
  return run();
}

InterpretResult VM::run() {
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << "          ";
    stack.print();
    std::cout << '\n';
    chunk.disassembleInstruction(programCounter);
#endif // DEBUG_TRACE_EXECUTION
    uint8_t instruction;
    switch (instruction = readByte()) {
      case OP_CONSTANT: {
        Value constant = readConstant();
        stack.push(constant);
        break;
      }
      // Q: how can I write this so I don't have to pass in 0?
      case OP_NULL: stack.push(Value{ VAL_NULL, 0 }); break;
      case OP_TRUE: stack.push(Value{ true }); break;
      case OP_FALSE: stack.push(Value{ false }); break;
      case OP_POP: stack.pop(); break;
      case OP_GET_LOCAL: {
        uint8_t slot = readByte();
        stack.push(stack.at(slot));
        break;
      }

      case OP_SET_LOCAL: {
        uint8_t slot = readByte();
        stack.set(slot, stack.peek(0));
        break;
      }

      case OP_GET_GLOBAL: {
        StringObject* name = readString();
        Value value;
        std::unordered_map<std::string, Value>::iterator it = globals.find(name->getChars());
        if (it == globals.end()) {
          runtimeError("Undefined variable '%s'.", name->getChars().c_str());
          return INTERPRET_RUNTIME_ERROR;
        } else {
          value = it->second;
        }
        stack.push(value);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        StringObject* name = readString();
        globals.insert(std::make_pair(name->getChars(), stack.peek(0)));
        stack.pop();
        break;
      }

      case OP_SET_GLOBAL: {
        StringObject* name = readString();
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
          runtimeError("Operands must be numbers.");
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

      case OP_RETURN: {
        // Exit interpreter.
        return INTERPRET_OK;
      }
    }
  }
}

uint8_t VM::readByte() {
  return chunk.getBytecode().at(programCounter++);
}

Value VM::readConstant() {
  return chunk.getConstants().at(readByte());
}

StringObject* VM::readString() {
  return readConstant().asString();
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

  int line = chunk.getLine(programCounter - 1);
  std::cerr << "[line " << line << "] in script\n";

  stack.reset();
}

bool VM::valuesEqual(Value a, Value b) {
  if (a.getType() != b.getType()) return false;

  switch (a.getType()) {
    case VAL_BOOL:   return a.asBool() == b.asBool();
    case VAL_NULL:    return true;
    case VAL_NUMBER: return a.asNumber() == b.asNumber();
    case VAL_OBJECT: {
      StringObject* aString = a.asString();
      StringObject* bString = b.asString();

      return (*aString).getChars().compare((*bString).getChars()) == 0;
    }
    default:
      return false;
  }
}

// Q: do you ensure you free all types of objects that might still be lingering in memory
// when the VM finishes running?
void VM::freeObjects() {
  Object* object = objects;

  // Q: should this be nullptr or is NULL fine?
  while (object != NULL) {
    Object* next = object->getNext();
    free(object);
    object = next;
  }
}

void VM::setChunk(Chunk chunk) {
  this->chunk = chunk;
}

void VM::setObjects(Object* objects) {
  this->objects = objects;
}

void VM::resetProgramCounter() {
  programCounter = 0;
}
