#ifndef VM_H
#define VM_H

#include <unordered_map>

#include "chunk.h"
#include "stack.h"
#include "value.h"
#include "closureobject.h"
#include "nativeobject.h"
#include "classobject.h"

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILATION_ERROR,
  INTERPRET_RUNTIME_ERROR
};

struct CallFrame {
  ClosureObject* closure;
  int functionProgramCounter;
  Value* slots; // Q: would it be better to use std::array or sth other than pointers?
};

class VM {
  static const int callFramesMax = 64;
  CallFrame callFrames[callFramesMax];
  int callFrameCount;

  Stack stack;

  // Q: should this be initialised in a/the constructor?
  // Q: should the first template type be std::string?
  std::unordered_map<std::string, Value> globals; // Q: should this be a pointer?

  std::unordered_map<std::string, Value> strings; // Q: Is this needed? // Q: should this be a pointer?

  // Q: is there a better, more C++ way of keeping track of the list of
  // heap allocated objects?
  Object* objects;

  UpvalueObject* openUpvalues;

  uint8_t readByte(CallFrame* frame);
  uint16_t readShort(CallFrame* frame);
  Value readConstant(CallFrame* frame);
  StringObject* readString(CallFrame* frame);
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

  bool callValue(Value callee, int argCount);
  bool call(ClosureObject* closure, int argCount);
  UpvalueObject* captureUpvalue(Value* local);
  void closeUpvalues(Value* last);
  void defineMethod(StringObject* name);
  bool bindMethod(ClassObject* klass, StringObject* name);
  //void defineNative(const std::string &name, nativeFunctionPointer function); // Q: try to get this to work?
  StringObject* copyString(const std::string &name);

public:
  VM();
  VM(Object* objects);

  InterpretResult interpret();
  InterpretResult run();

  // Todo: get rid of this when all the code is working.
  // Q: how should Chunk be passed?
  void setChunk(Chunk chunk);

  void setObjects(Object* objects);

  // Todo: get rid of this when all the code is working.
  void resetProgramCounter();

  // Q: who should own this method?
  void freeObjects();

  void incrementCallFrameCount();

  //Value clockNative(int argCount, Value* args); // Q: should this be here? Should it be public?

  Stack* getStack();
  CallFrame* getCallFrames();
  int getCallFrameCount();
};

#endif // VM_H
