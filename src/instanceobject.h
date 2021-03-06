#ifndef INSTANCE_OBJECT_H
#define INSTANCE_OBJECT_H

#include "object.h"
#include "value.h"
#include "classobject.h"

#include <memory>
#include <unordered_map>

class InstanceObject : public Object {
  std::shared_ptr<ClassObject> klass;
  std::unordered_map<std::string, Value> fields;

public:
  InstanceObject(std::shared_ptr<ClassObject> klass);

  bool hasField(const std::string &name);

  Value getField(const std::string &name);
  void setField(const std::string &name, const Value &value);
  ClassObject* getKlass();
};

#endif // INSTANCE_OBJECT_H
