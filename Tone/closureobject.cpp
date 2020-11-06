#include "closureobject.h"

ClosureObject::ClosureObject() : Object{ OBJECT_CLOSURE }, function{ nullptr } {}

ClosureObject::ClosureObject(FunctionObject* function) : Object{ OBJECT_CLOSURE } {
  this->function = function; // Q: is there a better way to assign function to function?
}

void ClosureObject::setUpvalue(int index, UpvalueObject* upvalue) {
  if (index < 0) {
    return; // Q: ok for now. What's the best thing to do long-term?
  } else if (index >= (int)upvalues.size()) { // Q: is the cast ok?
    upvalues.push_back(upvalue);
  } else {
    upvalues.at(index) = upvalue;
  }
}

UpvalueObject* ClosureObject::getUpvalue(int index) {
  // Q: error handling?

  return upvalues.at(index);
}

FunctionObject* ClosureObject::getFunction() {
  return function;
}
