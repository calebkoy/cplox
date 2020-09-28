#ifndef VALUE_H
#define VALUE_H

#include <iostream>

typedef enum {
  VAL_BOOL,
  VAL_NULL,
  VAL_NUMBER,
} ValueType;

class Value {
  ValueType type;

  // Q: is union part of the official C++ standard?
  // This webpage suggests it isn't: https://gamedev.net/forums/topic/517279-c-making-a-union-inside-a-class/4362327/
  // That might affect portability
  union {
    bool boolean;
    double number;
  } as;

public:
  Value();
  Value(bool boolean);
  Value(ValueType type, double number);
  bool asBool();
  double asNumber();
  bool isBool();
  bool isNull();
  bool isNumber();
  //void print();
  friend std::ostream& operator<<(std::ostream& out, const Value &value) {
    switch (value.type) {
      case VAL_BOOL:
        value.as.boolean ? out << "true" : out << "false";
        break;
      case VAL_NULL: out << "null"; break;
      case VAL_NUMBER: out << value.as.number; break;
      default: break; // Q: should this be handled differently?
    }

    return out;
  }
};

//std::ostream& operator<<(std::ostream& out, const Value &value);

#endif // VALUE_H
