#ifndef VALUE_H
#define VALUE_H

#include "object.h"
#include "stringobject.h"

#include <iostream>
#include <memory>

class Value {
  union {
    bool boolean;
    double number;
    std::shared_ptr<Object> object;
  };

public:
  enum class ValueType {
    VAL_BOOL,
    VAL_NUMBER,
    VAL_OBJECT,
    VAL_NULL,
    VAL_NONE
  } type = ValueType::VAL_NONE;

  Value();
  Value(bool boolean);
  Value(std::shared_ptr<Object> object);
  Value(ValueType type, double number = 0);

  Value(const Value& value);
  Value& operator=(const Value& value);

  ~Value();

  bool asBool() const;
  double asNumber() const;
  std::shared_ptr<Object> asObject() const;
  std::shared_ptr<StringObject> asString() const;
  bool isBool() const;
  bool isNull();
  bool isNumber() const;
  bool isObject() const;
  bool isString() const;
  bool isFunction() const;
  bool isClosure()const;
  bool isUpvalue() const;
  bool isClass() const;
  bool isBoundMethod() const;
  bool isInstance() const;
  bool isFalsey();
  ObjectType getObjectType() const;
  std::string getFunctionName() const;
  std::string getClosureFunctionName() const;
  std::string getInstanceClassName() const;
  std::string getClassName() const;
  std::string getBoundMethodName() const;

  ValueType getType() const;

  friend std::ostream& operator<<(std::ostream& out, const Value &value) {
    switch (value.type) {
      case ValueType::VAL_BOOL:
        value.asBool() ? out << "true" : out << "false";
        break;
      case ValueType::VAL_NULL: out << "null"; break;
      case ValueType::VAL_NUMBER: out << value.asNumber(); break;
      case ValueType::VAL_OBJECT:
        if (value.isString()) {
          out << *(value.asString());
        } else if (value.isFunction()) {
          out << value.getFunctionName();
        } else if (value.isClosure()) {
          out << value.getClosureFunctionName();
        } else if (value.isUpvalue()) {
          out << "upvalue";
        } else if (value.isInstance()) {
          out << value.getInstanceClassName();
        } else if (value.isClass()) {
          out << value.getClassName();
        } else if (value.isBoundMethod()) {
          out << "<fn " << value.getBoundMethodName() << ">";
        } else {
          out << "Unknown ObjectType";
        }
        break;
      case ValueType::VAL_NONE:
        out << "This Value currently has no type";
        break;
      default:
        out << "Unknown ValueType";
        break;
    }

    return out;
  }
};

#endif // VALUE_H
