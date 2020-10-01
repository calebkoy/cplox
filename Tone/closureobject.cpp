#include "closureobject.h"

ClosureObject::ClosureObject() : Object{ OBJECT_CLOSURE }, function{ nullptr } {}

ClosureObject::ClosureObject(FunctionObject* function) : Object{ OBJECT_CLOSURE } {
  this->function = function; // Q: is there a better way to assign function to function?
}

FunctionObject* ClosureObject::getFunction() {
  return function;
}
