#include <iostream>
#include <stdarg.h> // Q: is this needed (for runtimeError())?

#include "vm.h"

#define DEBUG_TRACE_EXECUTION

VM::VM(Chunk chunk) : chunk{ chunk } {}

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
      case OP_ADD:
        if (!stack.peek(0).isNumber() || !stack.peek(1).isNumber()) {
          runtimeError("Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
        }
        add();
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
      case OP_NEGATE: {
        if (!stack.peek(0).isNumber()) {
          runtimeError("Operand must be a number."); // Q: should this be handled by ErrorReporter?
          return INTERPRET_RUNTIME_ERROR;
        }

        double negatedValue = -(stack.pop().asNumber());
        stack.push(Value{ VAL_NUMBER, negatedValue });
        break;
      }
      case OP_RETURN: {
        std::cout << stack.pop() << '\n';
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
