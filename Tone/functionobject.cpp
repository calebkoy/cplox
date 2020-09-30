#include "functionobject.h"

FunctionObject::FunctionObject() : Object{ OBJECT_FUNCTION }, arity{ 0 }, name{ nullptr } {}

StringObject* FunctionObject::getName() {
  return name;
}

Chunk* FunctionObject::getChunk() {
  return &chunk;
}
