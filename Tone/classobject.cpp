#include "classobject.h"

ClassObject::ClassObject() : Object{ OBJECT_CLASS }, name{ nullptr } {}

ClassObject::ClassObject(StringObject* name) : Object{ OBJECT_CLASS } {
  this->name = name;
}

StringObject* ClassObject::getName() {
  return name;
}
