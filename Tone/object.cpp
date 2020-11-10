#include "object.h"

Object::Object(ObjectType type) : type{ type }, next{ nullptr } {}

ObjectType Object::getType() {
  return type;
}

// Q: is this only related to GC? If so, remove.
Object* Object::getNext() {
  return next; // Q: is there a better way to return a pointer?
}
