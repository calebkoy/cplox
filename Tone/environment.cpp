#include "environment.h"

// Q: is this default constructor necessary?
Environment::Environment() : scopeDepth{ 0 }, localCount{ 0 },
                             function{ nullptr }, functionType{ TYPE_SCRIPT }, enclosing{ nullptr } {

  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?

  // Q: is token copied when it's put inside local?
  // If not, what happens to token when the constructor ends?
  Token token;
  token.lexeme = "";
  Local local = { token, 0, false };
  locals.at(localCount++) = local;
}

// Todo: reduce duplication in constructors. Potentially have a single constructor,
// depending on how both are used.
// Q: should move semantics and smart pointers be used for initialising `environment`?
// Q: if you don't implement a garbage collector, is it necessary to set function to nullptr
// then set it to a new FunctionObject immediately after?
Environment::Environment(FunctionType type, Environment* environment) : scopeDepth{ 0 }, localCount{ 0 },
                             function{ nullptr }, functionType{ type }, enclosing{ environment } {
  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?

  // Q: is token copied when it's put inside local?
  // If not, what happens to token when the constructor ends?
  Token token;
  if (type != TYPE_FUNCTION) {
    token.lexeme = "this";
    token.length = 4; // Q: is it necessary to set length?
  } else {
    token.lexeme = "";
    token.length = 0; // Q: is it necessary to set length?
  }

  Local local = { token, 0, false };
  locals.at(localCount++) = local;
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

Upvalue* Environment::getUpvalue(int index) {
  // Q: error checking? (e.g., guard clauses?)

  // Q: is returning by address fine?
  return &upvalues.at(index);
}

FunctionObject* Environment::getFunction() {
  return function;
}

bool Environment::localCountAtMax() {
  return localCount == uint8Count;
}

void Environment::addLocal(Token name) {
  Local local = { name, -1, false };
  locals[localCount++] = local;
}

Environment* Environment::getEnclosing() {
  return enclosing;
}

FunctionType Environment::getFunctionType() {
  return functionType;
}

int Environment::getUint8Count() {
  return uint8Count;
}
