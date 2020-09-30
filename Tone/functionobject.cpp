#include "functionobject.h"

FunctionObject::FunctionObject() : Object{ OBJECT_FUNCTION }, arity{ 0 }, name{ nullptr } {}

void FunctionObject::incrementArity() {
  arity++;
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

void FunctionObject::setName(StringObject* name) {
  this->name = name;
}
