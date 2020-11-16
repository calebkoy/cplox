#ifndef BOUND_METHOD_OBJECT_H
#define BOUND_METHOD_OBJECT_H

#include "object.h"
#include "value.h"
#include "closureobject.h"

#include <memory>

class BoundMethodObject : public Object {
  Value receiver;
  //ClosureObject* method; // Q: should this be a smart pointer?
  std::shared_ptr<ClosureObject> method;


public:
  BoundMethodObject(); // Q: is a default ctor needed?
  //BoundMethodObject(Value receiver, ClosureObject* method);
  BoundMethodObject(Value receiver, std::shared_ptr<ClosureObject> method);

  //ClosureObject* getMethod();
  std::shared_ptr<ClosureObject> getMethod();
  Value getReceiver();
};

#endif // BOUND_METHOD_OBJECT_H
