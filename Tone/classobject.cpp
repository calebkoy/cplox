#include "classobject.h"

ClassObject::ClassObject() : Object{ OBJECT_CLASS } {}

ClassObject::ClassObject(std::shared_ptr<StringObject> name) : Object{ OBJECT_CLASS } {
  this->name = name; // Q: should this be done using copy initialisation in the first line of the ctor?
}

Value ClassObject::getMethod(const std::string &name) {
  // Q: error handling?

  return methods.at(name);
}

bool ClassObject::findMethod(const std::string &name) {
  std::unordered_map<std::string, Value>::iterator it = methods.find(name);

  return it != methods.end();
}

void ClassObject::setMethod(const std::string &name, Value method) {
  // Q: error handling? Overriding?

  methods[name] = method;
}

std::shared_ptr<StringObject> ClassObject::getName() {
  return name;
}

std::unordered_map<std::string, Value>* ClassObject::getMethods() {
  return &methods;
}
