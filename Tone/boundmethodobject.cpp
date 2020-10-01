#include "boundmethodobject.h"

BoundMethodObject::BoundMethodObject() : Object{ OBJECT_BOUND_METHOD }, method{ nullptr } {}

BoundMethodObject::BoundMethodObject(Value receiver, ClosureObject* method) : Object{ OBJECT_BOUND_METHOD } {
  this->receiver = receiver;
  this->method = method;
}

ClosureObject* BoundMethodObject::getMethod() {
  return method;
}
