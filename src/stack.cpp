#include "stack.h"

#include <iostream>

Stack::Stack() : top{ arr.data() } {}

void Stack::reset() {
  top = arr.data();
}

void Stack::push(const Value &value) {
  *top = value;
  top++;
}

Value Stack::pop() {
  top--;
  return *top;
}

Value Stack::peek(int distance) {
  return *(top - 1 - distance);
}

Value Stack::at(int slot) {
  return arr.at(slot);
}

void Stack::set(int slot, const Value &value) {
  arr.at(slot) = value;
}

void Stack::print() {
  auto begin{ arr.begin() };
  for (auto p{ begin }; p < top; ++p) {
    std::cout << "[ " << *p << " ]";
  }
}

int Stack::getSize() {
  return static_cast<int>(arr.size());
}

Value* Stack::getTop() {
  return top;
}

void Stack::setTop(Value* value) {
  top = value;
}
