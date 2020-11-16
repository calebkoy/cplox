#include "boundmethodobject.h"

#include <utility>

BoundMethodObject::BoundMethodObject() : Object{ OBJECT_BOUND_METHOD } {}

BoundMethodObject::BoundMethodObject(Value receiver, std::shared_ptr<ClosureObject> method) :
  Object{ OBJECT_BOUND_METHOD }
{
  this->receiver = receiver;
  this->method = std::move(method);
}

//ClosureObject* BoundMethodObject::getMethod() {
std::shared_ptr<ClosureObject> BoundMethodObject::getMethod() {
  return method;
}

Value BoundMethodObject::getReceiver() {
  return receiver;
}
