#include "object.h"

Object::Object(ObjectType type) : type{ type } {}

ObjectType Object::getType() {
  return type;
}
