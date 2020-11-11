#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "constants.h"
#include "error_reporter.h"
#include "functionobject.h"
#include "scanner.h"

#include <array> // TODO: remove if you don't use
#include <memory> // TODO: for std::unique_ptr; remove if not using
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
  //static const int uint8Count = std::numeric_limits<uint8_t>::max() + 1;
  std::array<Local, constants::maxLocals> locals; // TODO: consider making this a std::vector too for consistency with upvalues
  //std::array<Upvalue, constants::maxLocals> upvalues;

  std::vector<Upvalue> upvalues;

  int scopeDepth;
  int localCount;
  FunctionObject* function;
  FunctionType functionType; // Q: could this go in the FunctionObject class?
  //Environment* enclosing;
  std::unique_ptr<Environment> enclosing;
  ErrorReporter &reporter;


public:
  Environment();
  //Environment(FunctionType type, Environment* environment);
  Environment(FunctionType type, std::unique_ptr<Environment> enclosing,
              ErrorReporter &reporter);

  void incrementScopeDepth();
  void decrementScopeDepth();
  void decrementLocalCount();
  void addLocal(Token name);
  bool localCountAtMax();
  Local* getLocal(int index);
  Upvalue* getUpvalue(int index);

  //TODO: remove if not needed
  //static int getUint8Count();

  int getScopeDepth();
  int getLocalCount();
  FunctionObject* getFunction(); // TODO: at the end of it all, remove this if it's not needed
  //Environment* getEnclosing(); // TODO: remove if no longer necessary
  std::unique_ptr<Environment> releaseEnclosing();
  //std::array<Upvalue, constants::maxLocals> releaseUpvalues();
  std::vector<Upvalue> releaseUpvalues();
  FunctionType getFunctionType();

  int addUpvalue(uint8_t index, bool isLocal, const Token &name);
  int resolveUpvalue(const Token &name);
  int resolveLocal(const Token &name);

  bool identifiersEqual(const Token &a, const Token &b);
};

#endif // ENVIRONMENT_H
