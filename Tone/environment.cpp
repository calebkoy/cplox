#include "environment.h"

#include <algorithm>

// Q: is this default constructor necessary?
//Environment::Environment() : scopeDepth{ 0 }, localCount{ 0 },
//                             function{ nullptr }, functionType{ TYPE_SCRIPT },
//                             enclosing{ nullptr }
//{
//  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?
//
//  // Q: is token copied when it's put inside local?
//  // If not, what happens to token when the constructor ends?
//  Token token;
//  token.lexeme = "";
//  Local local = { token, 0, false };
//  locals.at(localCount++) = local;
//}

// Todo: reduce duplication in constructors. Potentially have a single constructor,
// depending on how both are used.
// Q: should move semantics and smart pointers be used for initialising `environment`?
// Q: if you don't implement a garbage collector, is it necessary to set function to nullptr
// then set it to a new FunctionObject immediately after?
//Environment::Environment(FunctionType type, Environment* environment) : scopeDepth{ 0 }, localCount{ 0 },
//                             function{ nullptr }, functionType{ type }, enclosing{ environment } {
//  function = new FunctionObject(); // Q: how to make sure we avoid memory leaks?
//
//  // Q: is token copied when it's put inside local?
//  // If not, what happens to token when the constructor ends?
//  Token token;
//  if (type != TYPE_FUNCTION) {
//    token.lexeme = "this";
//    token.length = 4; // Q: is it necessary to set length?
//  } else {
//    token.lexeme = "";
//    token.length = 0; // Q: is it necessary to set length?
//  }
//
//  Local local = { token, 0, false };
//  locals.at(localCount++) = local;
//}

Environment::Environment(FunctionType type,
                         std::unique_ptr<Environment> enclosing,
                         ErrorReporter &reporter) :
                           scopeDepth{ 0 }, localCount{ 0 },
                          function{ nullptr }, functionType{ type },
                          enclosing{ std::move(enclosing) }, reporter{ reporter }
{
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
  //return localCount == uint8Count;
  return localCount == constants::maxLocals;
}

void Environment::addLocal(Token name) {
  Local local = { name, -1, false };
  locals[localCount++] = local;
}

// TODO: remove if no longer useful
//Environment* Environment::getEnclosing() {
//  return enclosing;
//}

std::unique_ptr<Environment> Environment::releaseEnclosing() {
  std::unique_ptr<Environment> returnValue;
  std::swap(returnValue, enclosing);
  return returnValue;
}

//std::array<Upvalue, constants::maxLocals> Environment::releaseUpvalues() {
//  std::array<Upvalue, constants::maxLocals> returnValue;
//  std::swap(returnValue, upvalues);
//  return returnValue;
//}

std::vector<Upvalue> Environment::releaseUpvalues() {
  std::vector<Upvalue> returnValue;
  std::swap(returnValue, upvalues);
  return returnValue;
}

FunctionType Environment::getFunctionType() {
  return functionType;
}

// TODO: remove if not needed
//int Environment::getUint8Count() {
//  return uint8Count;
//}

int Environment::addUpvalue(uint8_t index, bool isLocal, const Token &name) {
  int upvalueCount = getFunction()->getUpvalueCount();

  for (int i = 0; i < upvalueCount; i++) {
    Upvalue* upvalue = getUpvalue(i); // Q: do we need getUpvalue()? If not, just get rid of that method.
    if (upvalue->index == index && upvalue->isLocal == isLocal) {
      return i;
    }
  }

  if (upvalueCount == constants::maxLocals) {
    reporter.error(name, "Too many closure variables in function.");
    return 0;
  }

  // TODO: consider using the dynamic vector count if you stick with a dynamic vector
  // instead of an array. Then you might not need to use the function as an intermediary
  // Q: is there a cleaner way to add a new upvalue?
  //getUpvalue(upvalueCount)->isLocal = isLocal;
  //getUpvalue(upvalueCount)->index = index;
  upvalues.push_back({ index, isLocal });
  function->incrementUpvalueCount();

  return upvalueCount;
}

int Environment::resolveUpvalue(const Token &name) {
  if (enclosing == nullptr) return -1;

  int local = enclosing->resolveLocal(name);
  if (local != -1) {
    enclosing->getLocal(local)->isCaptured = true; // Q: will we need getLocal if it won't be used in Compiler or VM?
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
