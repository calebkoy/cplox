#include "classobject.h"

ClassObject::ClassObject() : Object{ OBJECT_CLASS }, name{ nullptr } {}

ClassObject::ClassObject(StringObject* name) : Object{ OBJECT_CLASS } {
  this->name = name;
}

void ClassObject::setMethod(const std::string &name, Value method) {
  // Q: error handling? Overriding?

  methods.insert(std::make_pair(name, method));
}

StringObject* ClassObject::getName() {
  return name;
}
