#ifndef CLOSURE_OBJECT_H
#define CLOSURE_OBJECT_H

#include "object.h"
#include "functionobject.h"
#include "upvalueobject.h"

#include <memory>
#include <vector>

// TODO: you prob need a custom copy ctor and a custom copy assignment op.
// Unless shared_ptr implements the copy semantics you need already...
class ClosureObject : public Object {
  //FunctionObject* function;
  std::shared_ptr<FunctionObject> function;
  //std::vector<UpvalueObject*> upvalues; // Q: what's the best practice for initialising a dynamic array of pointers to a type?
  std::vector<std::shared_ptr<UpvalueObject>> upvalues;

public:
  ClosureObject();
  //ClosureObject(FunctionObject* function);
  ClosureObject(std::shared_ptr<FunctionObject> function);

  //void setUpvalue(int index, UpvalueObject* upvalue);
  void setUpvalue(int index, std::shared_ptr<UpvalueObject> upvalue);
  //UpvalueObject* getUpvalue(int index);
  std::shared_ptr<UpvalueObject> getUpvalue(int index);

  FunctionObject* getFunction();
};

#endif // CLOSURE_OBJECT_H
