#include "object.h"

Object::Object(ObjectType type) : type{ type }, next{ nullptr } {}

ObjectType Object::getType() {
  return type;
}

void Object::setNext(Object* next) {
  this->next = next;
}

Object* Object::getNext() {
  return next; // Q: is there a better way to return a pointer?
}
