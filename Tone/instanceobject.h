#ifndef INSTANCE_OBJECT_H
#define INSTANCE_OBJECT_H

#include "object.h"
#include "value.h"
#include "classobject.h"

#include <memory>
#include <unordered_map>

class InstanceObject : public Object {
  //ClassObject* klass;
  std::shared_ptr<ClassObject> klass;
  std::unordered_map<std::string, Value> fields;

public:
  InstanceObject(); // Q: should the default constructor be deleted?
  //InstanceObject(ClassObject* klass);
  InstanceObject(std::shared_ptr<ClassObject> klass);

  bool hasField(const std::string &name);

  Value getField(const std::string &name);
  void setField(const std::string &name, Value value); // Q: should I pass value by value?
  ClassObject* getKlass();
};

#endif // INSTANCE_OBJECT_H
