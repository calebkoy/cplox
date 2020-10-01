#include "functionobject.h"

FunctionObject::FunctionObject() : Object{ OBJECT_FUNCTION }, arity{ 0 }, upvalueCount{ 0 }, name{ nullptr } {}

void FunctionObject::incrementArity() {
  arity++;
}

void FunctionObject::incrementUpvalueCount() {
  upvalueCount++;
}

StringObject* FunctionObject::getName() {
  return name;
}

Chunk* FunctionObject::getChunk() {
  return &chunk;
}

int FunctionObject::getArity() {
  return arity;
}

int FunctionObject::getUpvalueCount() {
  return upvalueCount;
}

void FunctionObject::setName(StringObject* name) {
  this->name = name;
}
