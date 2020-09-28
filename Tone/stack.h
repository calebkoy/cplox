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
  void print();
};

#endif // STACK_H
