#include "instanceobject.h"

#include <utility>

InstanceObject::InstanceObject(std::shared_ptr<ClassObject> klass) :
  Object{ ObjectType::OBJECT_INSTANCE }, klass(std::move(klass))
{
}

bool InstanceObject::hasField(const std::string &name) {
  std::unordered_map<std::string, Value>::iterator it = fields.find(name);
  return it != fields.end();
}

Value InstanceObject::getField(const std::string &name) {
  return fields.at(name);
}

void InstanceObject::setField(const std::string &name, const Value &value) {
  fields[name] = value;
}

ClassObject* InstanceObject::getKlass() {
  return klass.get();
}
