#ifndef UPVALUE_OBJECT_H
#define UPVALUE_OBJECT_H

#include "object.h"
#include "value.h"

class UpvalueObject : public Object {
  Value* location;
  Value closed;

  // Q: is there a better way of maintaining a list of upvalues w/o using an intrusive linked list?
  UpvalueObject* next;

public:
  UpvalueObject();
  UpvalueObject(Value* location);

  void setLocationValue(Value value);

  Value* getLocation();
  UpvalueObject* getNext();
  Value* getClosed();

  void setNext(UpvalueObject* next);
  void setClosed(Value closed);
  void setLocation(Value* location);
};

#endif // UPVALUE_OBJECT_H
