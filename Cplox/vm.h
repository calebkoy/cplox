#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "classobject.h"
#include "closureobject.h"
#include "constants.h"
#include "stack.h"
#include "value.h"

#include <memory>
#include <unordered_map>

enum class InterpreterResult {
  OK,
  COMPILATION_ERROR,
  RUNTIME_ERROR
};

struct CallFrame {
  ClosureObject* closure;
  int functionProgramCounter;
  Value* slots;
};

class VM {
  CallFrame callFrames[constants::callFramesMax];
  int callFrameCount{ 0 };
  Stack stack;
  std::unordered_map<std::string, Value> globals;
  std::shared_ptr<UpvalueObject> openUpvalues;
  std::unique_ptr<StringObject> initString;

  uint8_t readByte(CallFrame* frame);
  uint16_t readShort(CallFrame* frame);
  Value readConstant(CallFrame* frame);
  std::shared_ptr<StringObject> readString(CallFrame* frame);
  void add();
  void subtract();
  void divide();
  void multiply();
  void greaterThan();
  void lessThan();
  void runtimeError(const char* format, ...);
  bool valuesEqual(const Value &a, const Value &b);
  bool callValue(const Value &callee, int argCount);
  std::shared_ptr<UpvalueObject> captureUpvalue(Value* local);
  void closeUpvalues(Value* last);
  void defineMethod(StringObject* name);
  bool bindMethod(ClassObject* klass, StringObject* name);
  bool invoke(StringObject* name, int argCount);
  bool invokeFromClass(ClassObject* klass, StringObject* name, int argCount);
  StringObject* copyString(const std::string &name);

public:
  VM();

  InterpreterResult run();
  bool call(ClosureObject* closure, int argCount);
  void pushOntoStack(const Value &value);
  Value* getStackTop();
};

#endif // VM_H
