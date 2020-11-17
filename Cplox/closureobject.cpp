#include "closureobject.h"

ClosureObject::ClosureObject(std::shared_ptr<FunctionObject> function) :
  Object{ OBJECT_CLOSURE }, function(std::move(function))
{
}

void ClosureObject::setUpvalue(int index,
                               std::shared_ptr<UpvalueObject> upvalue) {

  if (index >= static_cast<int>(upvalues.size())) {
    upvalues.push_back(std::move(upvalue));
  } else {
    upvalues.at(index) = std::move(upvalue);
  }
}

std::shared_ptr<UpvalueObject> ClosureObject::getUpvalue(int index) {
  return upvalues.at(index);
}

FunctionObject* ClosureObject::getFunction() {
  return function.get();
}
