#include "closureobject.h"
#include "functionobject.h"
#include "instanceobject.h"
#include "value.h"
#include "boundmethodobject.h"

#include <iostream>
#include <utility> // for std::move. TODO: remove if not using move

// Q: does it matter that this logic was somewhat arbitrarily chosen?
// Q: is this ctor even necessary? Esp. if you're following the pattern in:
// https://stackoverflow.com/questions/24713833/using-a-union-with-unique-ptr
//Value::Value() : type{ VAL_NUMBER }
//{
//  as.number = 0;
//}
Value::Value() {} // Q: is it even necessary to write this here? would it be the same w/o?

Value::Value(bool boolean) : type{ VAL_BOOL }
{
  as.boolean = boolean;
}

// TODO: note that if you're simply going to get the raw pointer from
// the shared pointer, then you might as well just pass in the raw
// pointer instead
//Value::Value(std::shared_ptr<Object> object) : type{ VAL_OBJECT }
//{
//  as.object = object.get();
//}

//Value::Value(Object* object) : type{ VAL_OBJECT }
//{
//
//  as.object = object; // Q: is pointer assignment correct here? Could we have leaks and/or other issues?
//  //as.object = std::move(object);
//}

Value::Value(std::unique_ptr<Object> object)
{
  //as.object = std::move(object);
  new (&as.object) std::unique_ptr<Object>{ std::move(object) };
  type = VAL_OBJECT; // Q: is the unionType:: necessary?
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
  } else if (type == VAL_NULL) {
    as.number = 0;
  }

  // Q: what if someone passes in VAL_OBJ?

  this->type = type;
}

Value::~Value() {
  switch (type) {
    // Q: is the unionType:: prefix necessary?
    case VAL_BOOL:
    case VAL_NUMBER:
    case VAL_NULL:
      break;

    case VAL_OBJECT:
      as.object.~unique_ptr<Object>();
      break;
  }
}

bool Value::asBool() const {
  return as.boolean;
}

double Value::asNumber() const {
  return as.number;
}

Object* Value::asObject() const {
  return as.object;
}

StringObject* Value::asString() const {
  // Q: what to do if the Value doesn't contain
  // a pointer to a valid StringObject on the heap?
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

bool Value::isObject() const {
  return type == VAL_OBJECT;
}

bool Value::isString() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_STRING;
}

bool Value::isFunction() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_FUNCTION;
}

bool Value::isNative() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_NATIVE;
}

bool Value::isUpvalue() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_UPVALUE;
}

bool Value::isClosure() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_CLOSURE;
}

bool Value::isClass() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_CLASS;
}

bool Value::isInstance() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_INSTANCE;
}

bool Value::isBoundMethod() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && asObject()->getType() == OBJECT_BOUND_METHOD;
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

std::string Value::getFunctionName() const {
  StringObject* name = ((FunctionObject*)asObject())->getName();
  if (name == NULL) return "<script>"; // Q: could this be == nullptr?

  return name->getChars();
}

std::string Value::getClassName() const {
  return ((ClassObject*)asObject())->getName()->getChars();
}

std::string Value::getClosureFunctionName() const {
  ClosureObject* closure = (ClosureObject*)asObject();

  if (closure->getFunction() == NULL) {
    return "";
  } else if (closure->getFunction()->getName() == NULL) {
    return "<script>";
  } else {
    return "<fn " + closure->getFunction()->getName()->getChars() + ">";
  }
}

std::string Value::getInstanceClassName() const {
  InstanceObject* instance = (InstanceObject*)asObject();
  return instance->getKlass()->getName()->getChars() + " instance";
}

std::string Value::getBoundMethodName() const {
  BoundMethodObject* boundMethod = (BoundMethodObject*)asObject();

  // Q: is it possible for any of these pointers to be null?
  return boundMethod->getMethod()->getFunction()->getName()->getChars();
}
