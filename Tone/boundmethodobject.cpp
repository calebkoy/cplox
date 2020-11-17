#include "boundmethodobject.h"

#include <utility>

BoundMethodObject::BoundMethodObject(Value receiver,
                                     std::shared_ptr<ClosureObject> method) :
  Object{ OBJECT_BOUND_METHOD }, receiver{ receiver }, method(std::move(method))
{
}

std::shared_ptr<ClosureObject> BoundMethodObject::getMethod() {
  return method;
}

Value BoundMethodObject::getReceiver() {
  return receiver;
}
