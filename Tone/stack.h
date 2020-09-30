#ifndef STACK_H
#define STACK_H

#include <array>

#include "value.h"

class Stack {
  static const int max{ 64 * 256 }; // Todo: clean this up by removing hardcoded values
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

  Value* getTop();

  void setTop(Value*);
};

#endif // STACK_H
