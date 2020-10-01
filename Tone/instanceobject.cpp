#include "instanceobject.h"

InstanceObject::InstanceObject() : Object{ OBJECT_INSTANCE }, klass{ nullptr } {}

InstanceObject::InstanceObject(ClassObject* klass) : Object{ OBJECT_INSTANCE } {
  this->klass = klass;
}

ClassObject* InstanceObject::getKlass() {
  return klass;
}
