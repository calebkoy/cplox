#ifndef STACK_H
#define STACK_H

#include <array>

#include "value.h"

class Stack {
  static const int max{ 256 };
//  Value arr[max];
  std::array<Value, max> arr;
  Value* top;

public:
  Stack();
  void reset();
  void push(Value value);
  Value pop();
  Value peek(int distance);
  Value at(int slot);
  void set(int slot, Value value); // Q: Is passing value by value best here?
  void print();
};

#endif // STACK_H
