#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "stack.h"
#include "value.h"
#include "closureobject.h"
#include "classobject.h"

#include <memory> // for std::shared_ptr TODO: remove if not using
#include <unordered_map>

// TODO: make class enum
enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILATION_ERROR,
  INTERPRET_RUNTIME_ERROR
};

struct CallFrame {
  // TODO: it might be good to make this a smart ptr (shared), but be careful not to create it
  // by passing in a pointer which is already owned by another smart ptr.
  // See https://www.learncpp.com/cpp-tutorial/15-6-stdshared_ptr/
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
  //UpvalueObject* openUpvalues;
  std::shared_ptr<UpvalueObject> openUpvalues;
  //StringObject* initString;
  std::unique_ptr<StringObject> initString;

  uint8_t readByte(CallFrame* frame); // TODO: consider passing this and all other functions that take ptr by const ref
  uint16_t readShort(CallFrame* frame);
  Value readConstant(CallFrame* frame);
  //StringObject* readString(CallFrame* frame);
  std::shared_ptr<StringObject> readString(CallFrame* frame);
  void add();
  void subtract();
  void divide();
  void multiply();
  void greaterThan();
  void lessThan();

  void runtimeError(const char* format, ...);

  // todo: put this in the class that should own it (maybe in Value, by overriding
  // ==)
  bool valuesEqual(Value a, Value b); // TODO: consider passing by const ref; note that Value has a shared_ptr

  bool callValue(Value callee, int argCount); // TODO: consider passing by const ref; note that Value has a shared_ptr
  //UpvalueObject* captureUpvalue(Value* local); // TODO: consider passing by const ref; note that Value has a shared_ptr
  std::shared_ptr<UpvalueObject> captureUpvalue(Value* local); // TODO: consider passing by const ref; note that Value has a shared_ptr
  void closeUpvalues(Value* last); // TODO: consider passing by const ref; note that Value has a shared_ptr
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
  void pushOntoStack(const Value &value); // Q: is this fine to be const ref, even though Value contains a shared_ptr?
  Value* getStackTop();
};

#endif // VM_H
