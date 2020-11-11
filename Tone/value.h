#ifndef VALUE_H
#define VALUE_H

#include "object.h"
#include "stringobject.h"

#include <iostream>
#include <memory> // TODO: remove if not using std::unique_ptr

//enum class ValueType {
//  VAL_BOOL,
//  VAL_NULL,
//  VAL_NUMBER,
//  VAL_OBJECT,
//  VAL_NONE
//};

class Value {
  //ValueType type;

  // Q: is union part of the official C++ standard?
  // This webpage suggests it isn't: https://gamedev.net/forums/topic/517279-c-making-a-union-inside-a-class/4362327/
  // That might affect portability
  union {
    bool boolean;
    double number;
    //TODO: consider using: std::unique_ptr<Object> object;
    std::unique_ptr<Object> object;
    //Object* object;
  } as; // Q: should you use 'as' here? Does it have any real benefit? What would it be like w/o it?

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
  Value(std::unique_ptr<Object> object);
  //Value(Object* object); // TODO: get rid of this if the program works with a smart pointer
  Value(ValueType type, double number);

  ~Value();

  bool asBool() const;
  double asNumber() const;
  Object* asObject() const;
  StringObject* asString() const;

  bool isBool();
  bool isNull();
  bool isNumber();
  bool isObject() const;
  bool isString() const;
  bool isFunction() const;
  bool isClosure()const;
  bool isNative() const;
  bool isUpvalue() const;
  bool isClass() const;
  bool isBoundMethod() const;
  bool isInstance() const;
  bool isFalsey();
  ObjectType getObjectType();

  // Q: is this the right place for these? Especially bec. their main use is for printing.
  std::string getFunctionName() const;
  std::string getClosureFunctionName() const;
  std::string getInstanceClassName() const;
  std::string getClassName() const;
  std::string getBoundMethodName() const;

  ValueType getType();

  friend std::ostream& operator<<(std::ostream& out, const Value &value) {
    switch (value.type) {
      case ValueType::VAL_BOOL:
        value.asBool() ? out << "true" : out << "false";
        break;
      case ValueType::VAL_NULL: out << "null"; break;
      case ValueType::VAL_NUMBER: out << value.asNumber(); break;

      // Q: what's the cleaner way to do this using inheritance
      // and/or polymorphism?
      case ValueType::VAL_OBJECT:
        if (value.isString()) {
          out << *(value.asString());
        } else if (value.isFunction()) {
          //out << *(value.asFunction());
          out << value.getFunctionName();
        } else if (value.isNative()) {
          out << "<native fn>";
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
          // Q: related to the above question:
          // What, if anything, should be done here?
          //out << *(value.asObject());
        }
        break;
      default: break; // Q: should this be handled differently?
    }

    return out;
  }
};

//std::ostream& operator<<(std::ostream& out, const Value &value);

#endif // VALUE_H
