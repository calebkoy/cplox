#ifndef CLOSURE_OBJECT_H
#define CLOSURE_OBJECT_H

#include "object.h"
#include "functionobject.h"

class ClosureObject : public Object {
  FunctionObject* function;

public:
  ClosureObject();
  ClosureObject(FunctionObject* function);

  FunctionObject* getFunction();
};

#endif // CLOSURE_OBJECT_H
