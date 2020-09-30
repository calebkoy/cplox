#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <array>
#include <limits>

#include "functionobject.h"
#include "scanner.h"

typedef struct {
  Token name;
  int depth;
} Local;

class Environment {
  static const int uint8Count = std::numeric_limits<uint8_t>::max();
  std::array<Local, uint8Count+1> locals;
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

  int getScopeDepth();
  int getLocalCount();
  FunctionObject* getFunction();
  Environment* getEnclosing();
};

#endif // ENVIRONMENT_H
