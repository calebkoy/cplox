#include <iostream>

#include "value.h"

// Q: does it matter that this logic was somewhat arbitrarily chosen?
Value::Value() {
  type = VAL_NUMBER;
  as.number = 0;
}

Value::Value(bool boolean) {
  // Q: what to do if type != VAL_BOOL?
  this->type = VAL_BOOL;
  as.boolean = boolean;
}

Value::Value(Object* object) {
  // Q: what to do if type != VAL_OBJECT?

  this->type = VAL_OBJECT;
  as.object = object; // Q: is pointer assignment correct here? Could we have leaks and/or other issues?
  //as.object = std::move(object);
}

Value::Value(ValueType type, double number = 0) {
  if (type == VAL_BOOL) {
    if (number == 0) {
      as.boolean = false;
    } else {
      as.boolean = true;
    }
  } else if (type == VAL_NUMBER) {
    as.number = number;
  }

  // Q: what if someone passes in VAL_OBJ?

  this->type = type;
}

bool Value::asBool() {
  return as.boolean;
}

double Value::asNumber() {
  return as.number;
}

Object* Value::asObject() {
  return as.object;
}

StringObject* Value::asString() {
  // Q: what to do if the Value doesn't contain
  // a pointer to a valid ObjString on the heap?
  return (StringObject*)asObject();
}

bool Value::isBool() {
  return type == VAL_BOOL;
}

bool Value::isNull() {
  return type == VAL_NULL;
}

bool Value::isNumber() {
  return type == VAL_NUMBER;
}

bool Value::isObject() {
  return type == VAL_OBJECT;
}

bool Value::isString() {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_STRING;
}

bool Value::isFalsey() {
  return isNull() || (isBool() && !asBool());
}

ValueType Value::getType() {
  return type;
}

ObjectType Value::getObjectType() {
  // Q: what to do if value isn't of type object?
  return asObject()->getType();
}
