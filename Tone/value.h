#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>
#include <variant>

#include "object.h"
#include "stringobject.h"

typedef enum {
  VAL_BOOL,
  VAL_NULL,
  VAL_NUMBER,
  VAL_OBJECT
} ValueType;

class Value {
  ValueType type;

  // Q: is union part of the official C++ standard?
  // This webpage suggests it isn't: https://gamedev.net/forums/topic/517279-c-making-a-union-inside-a-class/4362327/
  // That might affect portability
  union {
    bool boolean;
    double number;
    //std::unique_ptr<Object> object;
    Object* object;
  } as;

//  std::variant<bool, double, std::unique_ptr<Object>> as;

public:
  Value();
  Value(bool boolean);
  // Todo: learn how to explain how unique_ptr and move semantics work
  // here and the benefits and potential drawbacks
  Value(Object* object);
  Value(ValueType type, double number);

  bool asBool();
  double asNumber();
  Object* asObject();
  StringObject* asString();

  bool isBool();
  bool isNull();
  bool isNumber();
  bool isObject();
  bool isString();
  bool isFalsey();
  ValueType getType();
  ObjectType getObjectType();
  friend std::ostream& operator<<(std::ostream& out, const Value &value) {
    switch (value.type) {
      case VAL_BOOL:
        value.as.boolean ? out << "true" : out << "false";
        break;
      case VAL_NULL: out << "null"; break;
      case VAL_NUMBER: out << value.as.number; break;
      case VAL_OBJECT: out << value.as.object; break;
      default: break; // Q: should this be handled differently?
    }

    return out;
  }
};

//std::ostream& operator<<(std::ostream& out, const Value &value);

#endif // VALUE_H
