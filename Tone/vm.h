#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "stack.h"
#include "value.h"
#include "closureobject.h"
#include "nativeobject.h"
#include "classobject.h"

#include <unordered_map>

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
  UpvalueObject* openUpvalues;
  StringObject* initString;

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

  void runtimeError(const char* format, ...);

  // todo: put this in the class that should own it (maybe in Value, by overriding
  // ==)
  bool valuesEqual(Value a, Value b);

  bool callValue(Value callee, int argCount);
  UpvalueObject* captureUpvalue(Value* local);
  void closeUpvalues(Value* last);
  void defineMethod(StringObject* name);
  bool bindMethod(ClassObject* klass, StringObject* name);
  bool invoke(StringObject* name, int argCount);
  bool invokeFromClass(ClassObject* klass, StringObject* name, int argCount);
  StringObject* copyString(const std::string &name);

public:
  VM();

  InterpretResult interpret();
  InterpretResult run();
  bool call(ClosureObject* closure, int argCount);
  void pushOntoStack(const Value &value);
  Value* getStackTop();
};

#endif // VM_H
