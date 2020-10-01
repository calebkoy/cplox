#ifndef CLOSURE_OBJECT_H
#define CLOSURE_OBJECT_H

#include <vector>

#include "object.h"
#include "functionobject.h"
#include "upvalueobject.h"

class ClosureObject : public Object {
  FunctionObject* function;
  std::vector<UpvalueObject*> upvalues; // Q: what's the best practice for initialising a dynamic array of pointers to a type?

public:
  ClosureObject();
  ClosureObject(FunctionObject* function);

  void setUpvalue(int index, UpvalueObject* upvalue);
  UpvalueObject* getUpvalue(int index);

  FunctionObject* getFunction();
};

#endif // CLOSURE_OBJECT_H
