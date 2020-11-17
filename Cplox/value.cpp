#include "closureobject.h"
#include "functionobject.h"
#include "instanceobject.h"
#include "value.h"
#include "boundmethodobject.h"

#include <iostream>
#include <memory>
#include <utility> // for std::move. TODO: remove if not using move

// Q: does it matter that this logic was somewhat arbitrarily chosen?
// Q: is this ctor even necessary? Esp. if you're following the pattern in:
// https://stackoverflow.com/questions/24713833/using-a-union-with-unique-ptr
//Value::Value() : type{ VAL_NUMBER }
//{
//  as.number = 0;
//}
Value::Value() {} // Q: is it even necessary to write this here? would it be the same w/o?

Value::Value(bool boolean) : type{ ValueType::VAL_BOOL }
{
  //as.boolean = boolean;
  this->boolean = boolean;
  //variant = boolean;
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

Value::Value(std::shared_ptr<Object> o)
{
  //as.object = std::move(object);

  //new (&as.object) std::unique_ptr<Object>{ std::move(object) };
  //new (&object) std::unique_ptr<Object>{ std::move(o) };
  new (&object) std::shared_ptr<Object>(std::move(o));
  //variant = std::move(o);
  type = ValueType::VAL_OBJECT; // Q: is the unionType:: necessary?
}

Value::Value(ValueType type, double number) {
  if (type == ValueType::VAL_BOOL) {
    if (number == 0) {
      //as.boolean = false;
      boolean = false;
      //variant = false;
    } else {
      //as.boolean = true;
      boolean = true;
      //variant = true;
    }
  } else if (type == ValueType::VAL_NUMBER) {
    //as.number = number;
    this->number = number;
    //variant = number;
  } else if (type == ValueType::VAL_NULL) {
    //as.number = 0;
    number = 0;
    //variant = 0;
  }

  // Q: what if someone passes in VAL_OBJ? A: TBC. prob call other ctor
  // Q: what if someone passes in VAL_NONE? A: TBC.

  this->type = type;
}

Value::Value(const Value& value) {
  //variant = value.variant;

  // If using union and not using std::variant // TODO: remove comment
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
  //return as.boolean;
  return boolean;

  // Q: should there be a check that type is VAL_BOOL before returning?
  //return variant;
}

double Value::asNumber() const {
  //return as.number;
  return number;

  // Q: should there be a check that type is VAL_NUMBER before returning?
  //return variant;
}

// TODO: fix this. Perhaps you don't want to
// return a std::unique_ptr, because that will
// move ownership. What should you do instead?
// Q: is this method even needed anymore? Are any of the
// as*** methods even needed? Maybe we just need one
// since we're using shared_ptr.
//Object* Value::asObject() const {
std::shared_ptr<Object> Value::asObject() const {
  //return as.object;
  //return object.get();
  return object;

  // Q: should there be a check that type is VAL_OBJECT before returning?
  //return variant.get();
}

//StringObject* Value::asString() const {
std::shared_ptr<StringObject> Value::asString() const {
  // Q: what to do if the Value doesn't contain
  // a pointer to a valid StringObject on the heap?
  //return (StringObject*)asObject();

  // If object doesn't contain a valid StringObject,
  // throw an error.
  if (!isString()) {
    throw "Value must be StringObject in order to call asString()!";
  }
  return std::static_pointer_cast<StringObject>(object);
}

// TODO: prob remove; is causing errors due to circular references
//std::shared_ptr<FunctionObject> Value::asFunction() const {
//  if (!isFunction()) {
//    throw "Value must be FunctionObject in order to call asFunction()!";
//  }
//  return std::static_pointer_cast<FunctionObject>(object);
//}

bool Value::isBool() {
  return type == ValueType::VAL_BOOL;
}

bool Value::isNull() {
  return type == ValueType::VAL_NULL;
}

bool Value::isNumber() {
  return type == ValueType::VAL_NUMBER;
}

bool Value::isObject() const {
  return type == ValueType::VAL_OBJECT;
}

bool Value::isString() const {
  // Q: should some/all of this functionality belong to class Object?
  //return isObject() && asObject()->getType() == OBJECT_STRING;
  return isObject() && object->getType() == OBJECT_STRING;
}

// TODO: prob need to change this and all other is***() methods
// since we're using shared_ptr now
bool Value::isFunction() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_FUNCTION;
}

bool Value::isUpvalue() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_UPVALUE;
}

bool Value::isClosure() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_CLOSURE;
}

bool Value::isClass() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_CLASS;
}

bool Value::isInstance() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_INSTANCE;
}

bool Value::isBoundMethod() const {
  // Q: should some/all of this functionality belong to class Object?
  return isObject() && object->getType() == OBJECT_BOUND_METHOD;
}

bool Value::isFalsey() {
  return isNull() || (isBool() && !asBool());
}

Value::ValueType Value::getType() {
  return type;
}

ObjectType Value::getObjectType() {
  // Q: what to do if value isn't of type object?
  return object->getType();
}

std::string Value::getFunctionName() const {
  if (!isFunction()) {
    throw "Value must be function in order to call getFunctionName()!";
  }
  StringObject* name = std::static_pointer_cast<FunctionObject>(object)->getName();
  if (name == nullptr) return "<script>";
  return name->getChars();


//  StringObject* name = ((FunctionObject*)asObject())->getName();
//  if (name == nullptr) return "<script>";
//
//  return name->getChars();
}

std::string Value::getClassName() const {
  if (!isClass()) {
    throw "Value must be class in order to call getClassName()!";
  }
  return std::static_pointer_cast<ClassObject>(object)->getName()->getChars();

  //return ((ClassObject*)asObject())->getName()->getChars();
}

std::string Value::getClosureFunctionName() const {
  if (!isClosure()) {
    throw "Value must be closure in order to call getClosureFunctionName()!";
  }
  auto closure = std::static_pointer_cast<ClosureObject>(object);
  if (closure->getFunction() == nullptr) { // TODO: prob fix this since getFunction() will return smart ptr
    return "";
  } else if (closure->getFunction()->getName() == nullptr) {
    return "<script>";
  } else {
    return "<fn " + closure->getFunction()->getName()->getChars() + ">";
  }

//  ClosureObject* closure = (ClosureObject*)asObject();
//
//  if (closure->getFunction() == nullptr) {
//    return "";
//  } else if (closure->getFunction()->getName() == nullptr) {
//    return "<script>";
//  } else {
//    return "<fn " + closure->getFunction()->getName()->getChars() + ">";
//  }
}

std::string Value::getInstanceClassName() const {
  if (!isInstance()) {
    throw "Value must be Instance in order to call getInstanceClassName()!";
  }
  auto instance = std::static_pointer_cast<InstanceObject>(object);
  return instance->getKlass()->getName()->getChars() + " instance";

//  InstanceObject* instance = (InstanceObject*)asObject();
//  return instance->getKlass()->getName()->getChars() + " instance";
}

std::string Value::getBoundMethodName() const {
  if (!isBoundMethod()) {
    throw "Value must be BoundMethod in order to call getBoundMethodName()!";
  }
  auto boundMethod = std::static_pointer_cast<BoundMethodObject>(object);
  // Q: is it possible for any of these pointers to be null?
  return boundMethod->getMethod()->getFunction()->getName()->getChars();

  //BoundMethodObject* boundMethod = (BoundMethodObject*)asObject();

  // Q: is it possible for any of these pointers to be null?
  //return boundMethod->getMethod()->getFunction()->getName()->getChars();
}
