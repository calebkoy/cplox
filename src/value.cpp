#include "closureobject.h"
#include "functionobject.h"
#include "instanceobject.h"
#include "value.h"
#include "boundmethodobject.h"

#include <iostream>
#include <memory>
#include <utility>

Value::Value() {}

Value::Value(bool boolean) : boolean{ boolean }, type{ ValueType::VAL_BOOL }
{
}

Value::Value(std::shared_ptr<Object> o)
{
  new (&object) std::shared_ptr<Object>(std::move(o));
  type = ValueType::VAL_OBJECT;
}

Value::Value(ValueType type, double number) {
  if (type == ValueType::VAL_BOOL) {
    if (number == 0) {
      boolean = false;
    } else {
      boolean = true;
    }
  } else if (type == ValueType::VAL_NUMBER) {
    this->number = number;
  } else if (type == ValueType::VAL_NULL) {
    number = 0;
  } else if (type == ValueType::VAL_OBJECT) {
    new (&object) std::shared_ptr<Object>();
  } else {
    // VAL_NONE
  }

  this->type = type;
}

Value::Value(const Value& value) {
  switch (value.type) {
    case ValueType::VAL_NONE:
      break;
    case ValueType::VAL_BOOL:
      boolean = value.boolean;
      break;
    case ValueType::VAL_NUMBER:
      number = value.number;
      break;
    case ValueType::VAL_NULL:
      number = 0;
      break;
    case ValueType::VAL_OBJECT:
      new (&object) std::shared_ptr<Object>(value.object);
      break;
  }
  type = value.type;
}

Value& Value::operator=(const Value& value) {
  if (&value == this) {
    return *this;
  }

  switch (type) {
    case ValueType::VAL_NONE:
    case ValueType::VAL_BOOL:
    case ValueType::VAL_NUMBER:
    case ValueType::VAL_NULL:
      break;

    case ValueType::VAL_OBJECT:
      object.~shared_ptr();
      break;
  }

  type = ValueType::VAL_NONE;

  switch (value.type) {
    case ValueType::VAL_NONE:
      break;
    case ValueType::VAL_BOOL:
      boolean = value.boolean;
      break;
    case ValueType::VAL_NUMBER:
      number = value.number;
      break;
    case ValueType::VAL_NULL:
      number = 0;
      break;
    case ValueType::VAL_OBJECT:
      new (&object) std::shared_ptr<Object>(value.object);
      break;
  }

  type = value.type;
  return *this;
}

Value::~Value() {
  switch (type) {
    case ValueType::VAL_NONE:
    case ValueType::VAL_BOOL:
    case ValueType::VAL_NUMBER:
    case ValueType::VAL_NULL:
      break;

    case ValueType::VAL_OBJECT:
      object.~shared_ptr<Object>();
      break;
  }
}

bool Value::asBool() const {
  if (!isBool()) throw "Value is not boolean type";
  return boolean;
}

double Value::asNumber() const {
  if (!isNumber()) throw "Value is not number type";
  return number;
}

std::shared_ptr<Object> Value::asObject() const {
  if (!isObject()) throw "Value is not object type";
  return object;
}

std::shared_ptr<StringObject> Value::asString() const {
  if (!isString()) {
    throw "Value must be StringObject in order to call asString()!";
  }

  return std::static_pointer_cast<StringObject>(object);
}

bool Value::isBool() const {
  return type == ValueType::VAL_BOOL;
}

bool Value::isNull() {
  return type == ValueType::VAL_NULL;
}

bool Value::isNumber() const {
  return type == ValueType::VAL_NUMBER;
}

bool Value::isObject() const {
  return type == ValueType::VAL_OBJECT;
}

bool Value::isString() const {
  return isObject() && object->getType() == ObjectType::OBJECT_STRING;
}

bool Value::isFunction() const {
  return isObject() && object->getType() == ObjectType::OBJECT_FUNCTION;
}

bool Value::isUpvalue() const {
  return isObject() && object->getType() == ObjectType::OBJECT_UPVALUE;
}

bool Value::isClosure() const {
  return isObject() && object->getType() == ObjectType::OBJECT_CLOSURE;
}

bool Value::isClass() const {
  return isObject() && object->getType() == ObjectType::OBJECT_CLASS;
}

bool Value::isInstance() const {
  return isObject() && object->getType() == ObjectType::OBJECT_INSTANCE;
}

bool Value::isBoundMethod() const {
  return isObject() && object->getType() == ObjectType::OBJECT_BOUND_METHOD;
}

bool Value::isFalsey() {
  return isNull() || (isBool() && !asBool());
}

Value::ValueType Value::getType() const {
  return type;
}

ObjectType Value::getObjectType() const {
  if (!isObject()) throw "Value is not object type";
  return object->getType();
}

std::string Value::getFunctionName() const {
  if (!isFunction()) {
    throw "Value must be function in order to call getFunctionName()!";
  }

  StringObject* name = std::static_pointer_cast<FunctionObject>(object)->getName();
  if (name == nullptr) return "<script>";
  return name->getChars();
}

std::string Value::getClassName() const {
  if (!isClass()) {
    throw "Value must be class in order to call getClassName()!";
  }

  return std::static_pointer_cast<ClassObject>(object)->getName()->getChars();
}

std::string Value::getClosureFunctionName() const {
  if (!isClosure()) {
    throw "Value must be closure in order to call getClosureFunctionName()!";
  }

  auto closure = std::static_pointer_cast<ClosureObject>(object);
  if (closure->getFunction() == nullptr) {
    return "";
  } else if (closure->getFunction()->getName() == nullptr) {
    return "<script>";
  } else {
    return "<fn " + closure->getFunction()->getName()->getChars() + ">";
  }
}

std::string Value::getInstanceClassName() const {
  if (!isInstance()) {
    throw "Value must be Instance in order to call getInstanceClassName()!";
  }

  auto instance = std::static_pointer_cast<InstanceObject>(object);
  return instance->getKlass()->getName()->getChars() + " instance";
}

std::string Value::getBoundMethodName() const {
  if (!isBoundMethod()) {
    throw "Value must be BoundMethod in order to call getBoundMethodName()!";
  }

  auto boundMethod = std::static_pointer_cast<BoundMethodObject>(object);
  return boundMethod->getMethod()->getFunction()->getName()->getChars();
}
