#include "upvalueobject.h"
#include "value.h"

UpvalueObject::UpvalueObject(Value* location) :
  Object{ ObjectType::OBJECT_UPVALUE }, location{ location }
{
}

void UpvalueObject::setLocationValue(const Value &value) {
  *location = value;
}

Value* UpvalueObject::getLocation() {
  return location;
}

std::shared_ptr<UpvalueObject> UpvalueObject::getNext() {
  return next;
}

Value* UpvalueObject::getClosed() {
  return &closed;
}

void UpvalueObject::setNext(std::shared_ptr<UpvalueObject> next) {
  this->next = std::move(next);
}

void UpvalueObject::setClosed(const Value &closed) {
  this->closed = closed;
}

void UpvalueObject::setLocation(Value* location) {
  this->location = location;
}
