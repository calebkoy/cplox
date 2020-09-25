#include <iostream>

#include "stack.h"

Stack::Stack() : top{ arr.data() } {}

void Stack::reset() {
  top = arr.data();
}

void Stack::push(Value value) {
  *top = value;
  top++;
}

Value Stack::pop() {
  top--;
  return *top;
}

void Stack::print() {
  auto begin{ arr.begin() };

  for (auto p{ begin }; p < top; ++p) {
    std::cout << "[ " << *p << " ]";
  }
}
