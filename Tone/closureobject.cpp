#include "closureobject.h"

ClosureObject::ClosureObject() : Object{ OBJECT_CLOSURE } {}

ClosureObject::ClosureObject(std::shared_ptr<FunctionObject> function) : Object{ OBJECT_CLOSURE } {
  this->function = std::move(function); // Q: is there a better way to do assign/initialise here?
}

//void ClosureObject::setUpvalue(int index, UpvalueObject* upvalue) {
void ClosureObject::setUpvalue(int index, std::shared_ptr<UpvalueObject> upvalue) {
  if (index < 0) {
    return; // Q: ok for now. What's the best thing to do long-term?
  } else if (index >= (int)upvalues.size()) { // Q: is the cast ok?
    upvalues.push_back(std::move(upvalue));
  } else {
    upvalues.at(index) = std::move(upvalue);
  }
}

//UpvalueObject* ClosureObject::getUpvalue(int index) {
std::shared_ptr<UpvalueObject> ClosureObject::getUpvalue(int index) {
  // Q: error handling?

  return upvalues.at(index);
}

FunctionObject* ClosureObject::getFunction() {
  return function.get();
}
