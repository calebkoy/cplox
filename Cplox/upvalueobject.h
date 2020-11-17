#ifndef UPVALUE_OBJECT_H
#define UPVALUE_OBJECT_H

#include "object.h"
#include "value.h"

#include <memory>

class UpvalueObject : public Object {
  Value* location{ nullptr };
  Value closed;
  std::shared_ptr<UpvalueObject> next;

public:
  UpvalueObject(Value* location);

  void setLocationValue(const Value &value);

  Value* getLocation();
  std::shared_ptr<UpvalueObject> getNext();
  Value* getClosed();
  void setNext(std::shared_ptr<UpvalueObject> next);
  void setClosed(const Value &closed);
  void setLocation(Value* location);
};

#endif // UPVALUE_OBJECT_H
