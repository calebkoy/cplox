#ifndef UPVALUE_OBJECT_H
#define UPVALUE_OBJECT_H

#include "object.h"
#include "value.h"

#include <memory>

class UpvalueObject : public Object {
  Value* location;
  Value closed;

  // Q: is there a better way of maintaining a list of upvalues w/o using an intrusive linked list?
  //UpvalueObject* next;
  std::shared_ptr<UpvalueObject> next;

public:
  UpvalueObject();
  UpvalueObject(Value* location);

  void setLocationValue(Value value);

  Value* getLocation();
  //UpvalueObject* getNext();
  std::shared_ptr<UpvalueObject> getNext();
  Value* getClosed();

  //void setNext(UpvalueObject* next);
  void setNext(std::shared_ptr<UpvalueObject> next);
  void setClosed(Value closed);
  void setLocation(Value* location);
};

#endif // UPVALUE_OBJECT_H
