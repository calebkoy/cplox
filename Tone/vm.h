#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "stack.h"

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILATION_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
  Chunk chunk;
  int programCounter{ 0 };
  Stack stack;

  uint8_t readByte();
  Value readConstant();
  void add();
  void subtract();
  void divide();
  void multiply();

public:
  VM(Chunk chunk);
  InterpretResult interpret();
  InterpretResult run();
};

#endif // VM_H
