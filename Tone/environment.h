#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "constants.h"
#include "error_reporter.h"
#include "functionobject.h"
#include "scanner.h"

#include <array>
#include <memory>
#include <vector>

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
  std::array<Local, constants::maxLocals> locals;
  std::vector<Upvalue> upvalues;
  int scopeDepth{ 0 };
  int localCount{ 0 };
  FunctionObject::FunctionType functionType;
  std::unique_ptr<Environment> enclosing;
  ErrorReporter &reporter;
  std::shared_ptr<FunctionObject> function;

  bool identifiersEqual(const Token &a, const Token &b);

public:
  Environment(FunctionObject::FunctionType type,
              std::unique_ptr<Environment> enclosing,
              ErrorReporter &reporter);

  bool localCountAtMax();
  void incrementScopeDepth();
  void decrementScopeDepth();
  void decrementLocalCount();
  void addLocal(Token name);
  Local* getLocal(int index);
  Upvalue* getUpvalue(int index);

  int getScopeDepth();
  int getLocalCount();
  std::shared_ptr<FunctionObject> getFunction();
  std::unique_ptr<Environment> releaseEnclosing();
  std::vector<Upvalue> releaseUpvalues();
  FunctionObject::FunctionType getFunctionType();

  int addUpvalue(uint8_t index, bool isLocal, const Token &name);
  int resolveUpvalue(const Token &name);
  int resolveLocal(const Token &name);
};

#endif // ENVIRONMENT_H
