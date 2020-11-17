#ifndef BOUND_METHOD_OBJECT_H
#define BOUND_METHOD_OBJECT_H

#include "object.h"
#include "value.h"
#include "closureobject.h"

#include <memory>

class BoundMethodObject : public Object {
  Value receiver;
  std::shared_ptr<ClosureObject> method;

public:
  BoundMethodObject(const Value &receiver,
                    std::shared_ptr<ClosureObject> method);

  std::shared_ptr<ClosureObject> getMethod();
  Value getReceiver();
};

#endif // BOUND_METHOD_OBJECT_H
