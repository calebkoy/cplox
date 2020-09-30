#ifndef VM_H
#define VM_H

#include <unordered_map>

#include "chunk.h"
#include "stack.h"
#include "value.h"

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILATION_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
  Chunk chunk;
  int programCounter{ 0 };
  Stack stack;

  // Q: should this be initialised in a/the constructor?
  // Q: should the first template type be std::string?
  std::unordered_map<std::string, Value> globals;

  std::unordered_map<std::string, Value> strings; // Q: Is this needed?

  // Q: is there a better, more C++ way of keeping track of the list of
  // heap allocated objects?
  Object* objects;

  uint8_t readByte();
  uint16_t readShort();
  Value readConstant();
  StringObject* readString();
  void add();
  void subtract();
  void divide();
  void multiply();
  void greaterThan();
  void lessThan();

  // Q: how can I write this so it's handled by ErrorReporter?
  // Q: does this need to be variadic?
  void runtimeError(const char* format, ...);

  // todo: put this in the class that should own it (maybe in Value, by overriding
  // ==)
  bool valuesEqual(Value a, Value b);

public:
  VM();

  // Q: how should Chunk be passed?
  VM(Chunk chunk, Object* objects);

  InterpretResult interpret();
  InterpretResult run();

  // Q: how should Chunk be passed?
  void setChunk(Chunk chunk);

  void setObjects(Object* objects);
  void resetProgramCounter();

  // Q: who should own this method?
  void freeObjects();
};

#endif // VM_H
