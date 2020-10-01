#ifndef UPVALUE_OBJECT_H
#define UPVALUE_OBJECT_H

#include "object.h"
#include "value.h"

class UpvalueObject : public Object {
  Value* location;

public:
  UpvalueObject();
  UpvalueObject(Value* location);

  void setLocationValue(Value value);

  Value* getLocation();
};

#endif // UPVALUE_OBJECT_H
