#ifndef BOUND_METHOD_OBJECT_H
#define BOUND_METHOD_OBJECT_H

#include "object.h"
#include "value.h"
#include "closureobject.h"

class BoundMethodObject : public Object {
  Value receiver;
  ClosureObject* method;

public:
  BoundMethodObject(); // Q: is a default ctor needed?
  BoundMethodObject(Value receiver, ClosureObject* method);

  ClosureObject* getMethod();
};

#endif // BOUND_METHOD_OBJECT_H
