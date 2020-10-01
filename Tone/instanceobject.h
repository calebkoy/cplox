#ifndef INSTANCE_OBJECT_H
#define INSTANCE_OBJECT_H

#include <unordered_map>

#include "object.h"
#include "value.h"

class InstanceObject : public Object {
  ClassObject* klass;
  std::unordered_map<std::string, Value> fields;

public:
  InstanceObject(); // Q: should the default constructor be deleted?
  InstanceObject(ClassObject* klass);

  ClassObject* getKlass();
};

#endif // INSTANCE_OBJECT_H
