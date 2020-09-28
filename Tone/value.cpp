#include <iostream>

#include "value.h"

// Q: does it matter that this logic was somewhat arbitrarily chosen?
Value::Value() {
  type = VAL_NUMBER;
  as.number = 0;
}

Value::Value(bool boolean) : type{ VAL_BOOL } {
  as.boolean = boolean;
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

  this->type = type;
}

bool Value::asBool() {
  return as.boolean;
}

double Value::asNumber() {
  return as.number;
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
