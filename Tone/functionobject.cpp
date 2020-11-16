#include "functionobject.h"

// This commented out version is for when name is a StringObject* and not a shared_ptr
//FunctionObject::FunctionObject() : Object{ OBJECT_FUNCTION }, arity{ 0 }, upvalueCount{ 0 }, name{ nullptr } {}
FunctionObject::FunctionObject() : Object{ OBJECT_FUNCTION }, arity{ 0 }, upvalueCount{ 0 } {}

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

//void FunctionObject::setName(StringObject* name) {
//  this->name = name;
//}
void FunctionObject::setName(std::shared_ptr<StringObject> name) {
  this->name = name;
}
