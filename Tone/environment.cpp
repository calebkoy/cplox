#include "environment.h"

// Q: is this default constructor necessary?
Environment::Environment() : scopeDepth{ 0 }, localCount{ 0 },
                             function{ nullptr }, functionType{ TYPE_SCRIPT }, enclosing{ nullptr } {

  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?

  // Q: is token copied when it's put inside local?
  // If not, what happens to token when the constructor ends?
  Token token;
  token.lexeme = "";
  Local local = { token, 0 };
  localCount++;
}

// Todo: reduce duplication in constructors. Potentially have a single constructor,
// depending on how both are used.
// Q: should move semantics and smart pointers be used for initialising `environment`?
Environment::Environment(FunctionType type, Environment* environment) : scopeDepth{ 0 }, localCount{ 0 },
                             function{ nullptr }, functionType{ type }, enclosing{ environment } {
  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?

  // Q: is token copied when it's put inside local?
  // If not, what happens to token when the constructor ends?
  Token token;
  token.lexeme = "";
  Local local = { token, 0 };
  localCount++;
}

void Environment::incrementScopeDepth() {
  scopeDepth++;
}

void Environment::decrementScopeDepth() {
  scopeDepth--;
}

void Environment::decrementLocalCount() {
  localCount--;
}

int Environment::getScopeDepth() {
  return scopeDepth;
}

int Environment::getLocalCount() {
  return localCount;
}

Local* Environment::getLocal(int index) {
  // Q: error checking? (e.g., guard clauses?)

  // Q: is returning by address fine?
  return &locals.at(index);
}

FunctionObject* Environment::getFunction() {
  return function;
}

bool Environment::localCountAtMax() {
  return localCount == uint8Count;
}

void Environment::addLocal(Token name) {
  Local local = { name, -1 };
  locals[localCount++] = local;
}

Environment* Environment::getEnclosing() {
  return enclosing;
}

FunctionType Environment::getFunctionType() {
  return functionType;
}
