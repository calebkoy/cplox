#ifndef CLOSURE_OBJECT_H
#define CLOSURE_OBJECT_H

#include "functionobject.h"
#include "object.h"
#include "upvalueobject.h"

#include <memory>
#include <vector>

class ClosureObject : public Object {
  std::shared_ptr<FunctionObject> function;
  std::vector<std::shared_ptr<UpvalueObject>> upvalues;

public:
  ClosureObject(std::shared_ptr<FunctionObject> function);

  void setUpvalue(int index, std::shared_ptr<UpvalueObject> upvalue);
  std::shared_ptr<UpvalueObject> getUpvalue(int index);

  FunctionObject* getFunction();
};

#endif // CLOSURE_OBJECT_H
