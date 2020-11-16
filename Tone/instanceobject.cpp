#include "instanceobject.h"

#include <utility>

InstanceObject::InstanceObject() : Object{ OBJECT_INSTANCE } {}

InstanceObject::InstanceObject(std::shared_ptr<ClassObject> klass) : Object{ OBJECT_INSTANCE } {
  this->klass = std::move(klass);
}

bool InstanceObject::hasField(const std::string &name) {
  std::unordered_map<std::string, Value>::iterator it = fields.find(name);

  return it != fields.end();
}

Value InstanceObject::getField(const std::string &name) {
  // Q: Error handling?

  return fields.at(name);
}

void InstanceObject::setField(const std::string &name, Value value) {
  // Q: Error handling?

  fields[name] = value;
}

ClassObject* InstanceObject::getKlass() {
  return klass.get();
}
