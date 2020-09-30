#include "environment.h"

// Q: is this default constructor necessary?
Environment::Environment() : scopeDepth{ 0 }, localCount{ 0 } {}

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

bool Environment::localCountAtMax() {
  return localCount == uint8Count;
}

void Environment::addLocal(Token name) {
  Local local = { name, -1 };
  locals[localCount++] = local;
}
