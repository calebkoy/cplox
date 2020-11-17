#include "functionobject.h"

FunctionObject::FunctionObject() : Object{ ObjectType::OBJECT_FUNCTION } {}

void FunctionObject::incrementArity() {
  arity++;
}

void FunctionObject::incrementUpvalueCount() {
  upvalueCount++;
}

StringObject* FunctionObject::getName() {
  return name.get();
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

void FunctionObject::setName(std::shared_ptr<StringObject> name) {
  this->name = std::move(name);
}
