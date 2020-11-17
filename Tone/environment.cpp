#include "environment.h"

#include <algorithm>

Environment::Environment(FunctionObject::FunctionType type,
                         std::unique_ptr<Environment> enclosing,
                         ErrorReporter &reporter) :
                          functionType{ type },
                          enclosing{ std::move(enclosing) },
                          reporter{ reporter },
                          function{ std::make_shared<FunctionObject>() }
{
  Token token;
  if (type != FunctionObject::FunctionType::TYPE_FUNCTION) {
    token.lexeme = "this";
    token.length = 4;
  } else {
    token.lexeme = "";
    token.length = 0;
  }

  locals.at(localCount++) = { token, 0, false };
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
  return &locals.at(index);
}

Upvalue* Environment::getUpvalue(int index) {
  return &upvalues.at(index);
}

std::shared_ptr<FunctionObject> Environment::getFunction() {
  return function;
}

bool Environment::localCountAtMax() {
  return localCount == constants::maxLocals;
}

void Environment::addLocal(Token name) {
  locals.at(localCount++) = { name, -1, false };
}

std::unique_ptr<Environment> Environment::releaseEnclosing() {
  std::unique_ptr<Environment> returnValue;
  std::swap(returnValue, enclosing);
  return returnValue;
}

std::vector<Upvalue> Environment::releaseUpvalues() {
  std::vector<Upvalue> returnValue;
  std::swap(returnValue, upvalues);
  return returnValue;
}

FunctionObject::FunctionType Environment::getFunctionType() {
  return functionType;
}

int Environment::addUpvalue(uint8_t index, bool isLocal, const Token &name) {
  int upvalueCount = getFunction()->getUpvalueCount();
  for (int i = 0; i < upvalueCount; i++) {
    Upvalue* upvalue = getUpvalue(i);
    if (upvalue->index == index && upvalue->isLocal == isLocal) {
      return i;
    }
  }

  if (upvalueCount == constants::maxLocals) {
    reporter.error(name, "Too many closure variables in function.");
    return 0;
  }

  upvalues.push_back({ index, isLocal });
  function->incrementUpvalueCount();
  return upvalueCount;
}

int Environment::resolveUpvalue(const Token &name) {
  if (enclosing == nullptr) return -1;

  int local = enclosing->resolveLocal(name);
  if (local != -1) {
    enclosing->getLocal(local)->isCaptured = true;
    return addUpvalue(static_cast<uint8_t>(local), true, name);
  }

  int upvalue = enclosing->resolveUpvalue(name);
  if (upvalue != -1) {
    return addUpvalue(static_cast<uint8_t>(upvalue), false, name);
  }

  return -1;
}

int Environment::resolveLocal(const Token &name) {
  for (int i = localCount - 1; i >= 0; i--) {
    Local* local = getLocal(i);
    if (identifiersEqual(name, local->name)) {
      if (local->depth == -1) {
        reporter.error(name,
                       "Cannot read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}

bool Environment::identifiersEqual(const Token &a, const Token &b) {
  if (a.length != b.length) return false;
  return a.lexeme.compare(b.lexeme) == 0;
}
