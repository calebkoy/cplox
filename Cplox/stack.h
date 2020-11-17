#ifndef STACK_H
#define STACK_H

#include "value.h"

#include <array>

class Stack {
  static const int max{ 64 * 256 }; // Todo: clean this up by removing hardcoded values
  std::array<Value, max> arr;
  Value* top{ nullptr };

public:
  Stack();
  void reset();
  void push(Value value);
  Value pop();
  Value peek(int distance);
  Value at(int slot);
  void set(int slot, Value value); // Q: Is passing value by value best here?
  void print();
  int getSize();

  Value* getTop();

  void setTop(Value*);
};

#endif // STACK_H
