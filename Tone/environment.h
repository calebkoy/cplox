#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "functionobject.h"
#include "scanner.h"

#include <array>
#include <limits>

struct Local {
  Token name;
  int depth;
  bool isCaptured;
};

struct Upvalue {
  uint8_t index;
  bool isLocal;
};

class Environment {
  static const int uint8Count = std::numeric_limits<uint8_t>::max() + 1;
  std::array<Local, uint8Count> locals;
  std::array<Upvalue, uint8Count> upvalues;
  int scopeDepth;
  int localCount;
  FunctionObject* function;
  FunctionType functionType; // Q: could this go in the FunctionObject class?
  Environment* enclosing;

public:
  Environment();
  Environment(FunctionType type, Environment* environment);

  void incrementScopeDepth();
  void decrementScopeDepth();
  void decrementLocalCount();
  void addLocal(Token name);
  bool localCountAtMax();
  Local* getLocal(int index);
  Upvalue* getUpvalue(int index);

  static int getUint8Count();

  int getScopeDepth();
  int getLocalCount();
  FunctionObject* getFunction();
  Environment* getEnclosing();
  FunctionType getFunctionType();
};

#endif // ENVIRONMENT_H
