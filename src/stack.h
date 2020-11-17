#ifndef STACK_H
#define STACK_H

#include "constants.h"
#include "value.h"

#include <array>

class Stack {
  static const int stackMax{ 256 };
  static const int max{ constants::callFramesMax * stackMax };
  std::array<Value, max> arr;
  Value* top;

public:
  Stack();

  void reset();
  void push(const Value &value);
  Value pop();
  Value peek(int distance);
  Value at(int slot);
  void set(int slot, const Value &value);
  void print();
  int getSize();

  Value* getTop();
  void setTop(Value *value);
};

#endif // STACK_H
