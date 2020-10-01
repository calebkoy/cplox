#include "upvalueobject.h"

// Q: do I want this or should the default constructor be deleted?
// Ask the same question for your other classes.
UpvalueObject::UpvalueObject() : Object{ OBJECT_UPVALUE }, location{ nullptr } {}

UpvalueObject::UpvalueObject(Value* location) : Object{ OBJECT_UPVALUE } {
  this->location = location; // Q: is there a better way to assign the pointer?
}

void UpvalueObject::setLocationValue(Value value) {
  *location = value;
}

Value* UpvalueObject::getLocation() {
  return location;
}
