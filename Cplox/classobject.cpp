#include "classobject.h"

ClassObject::ClassObject(std::shared_ptr<StringObject> name) :
  Object{ OBJECT_CLASS }, name(std::move(name))
{
}

Value ClassObject::getMethod(const std::string &name) {
  return methods.at(name);
}

bool ClassObject::findMethod(const std::string &name) {
  std::unordered_map<std::string, Value>::iterator it = methods.find(name);
  return it != methods.end();
}

void ClassObject::setMethod(const std::string &name, const Value &method) {
  methods[name] = method;
}

std::shared_ptr<StringObject> ClassObject::getName() {
  return name;
}

std::unordered_map<std::string, Value>* ClassObject::getMethods() {
  return &methods;
}
