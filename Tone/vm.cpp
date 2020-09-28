#include <iostream>

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
      case OP_ADD: add(); break;
      case OP_SUBTRACT: subtract(); break;
      case OP_DIVIDE: divide(); break;
      case OP_MULTIPLY: multiply(); break;
      case OP_NEGATE: stack.push(-stack.pop()); break;
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
  double b = stack.pop();
  double a = stack.pop();
  stack.push(a + b);
}

void VM::subtract() {
  double b = stack.pop();
  double a = stack.pop();
  stack.push(a - b);
}

void VM::divide() {
  double b = stack.pop();
  double a = stack.pop();
  stack.push(a / b);
}

void VM::multiply() {
  double b = stack.pop();
  double a = stack.pop();
  stack.push(a * b);
}
