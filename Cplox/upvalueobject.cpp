#include "upvalueobject.h"
#include "value.h"

// Q: do I want this or should the default constructor be deleted?
// Ask the same question for your other classes.
UpvalueObject::UpvalueObject() : Object{ OBJECT_UPVALUE }, location{ nullptr }
{
  closed = Value( Value::ValueType::VAL_NULL );
}

UpvalueObject::UpvalueObject(Value* location) : Object{ OBJECT_UPVALUE } {
  this->location = location; // Q: is there a better way to assign the pointer?
}

void UpvalueObject::setLocationValue(Value value) {
  *location = value;
}

Value* UpvalueObject::getLocation() {
  return location;
}

//UpvalueObject* UpvalueObject::getNext() {
std::shared_ptr<UpvalueObject> UpvalueObject::getNext() {
  return next;
}

Value* UpvalueObject::getClosed() {
  return &closed;
}

void UpvalueObject::setNext(std::shared_ptr<UpvalueObject> next) {
  this->next = std::move(next);
}

void UpvalueObject::setClosed(Value closed) {
  this->closed = closed;
}

void UpvalueObject::setLocation(Value* location) {
  this->location = location;
}
