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

Value Stack::peek(int distance) {
  //Q: does std:array store its members in contiguous slots? If not, how do I fix this function?
  // Q: what's the best way to return? By pointer? By value?

  return *(top - 1 - distance);
}

// Q: is this the best way to return the value?
Value Stack::at(int slot) {
  return arr.at(slot); // Q: how to deal w/ error handling (guard clauses?)?
}

void Stack::set(int slot, Value value) {
  // Q: how to do error checking?
  arr.at(slot) = value;
}

void Stack::print() {
  auto begin{ arr.begin() };

  for (auto p{ begin }; p < top; ++p) {
    std::cout << "[ " << (*p) << " ]";
  }
}

int Stack::getSize() {
  return (int)arr.size(); // Q: is cast ok?
}

Value* Stack::getTop() {
  return top;
}

void Stack::setTop(Value* value) {
  top = value; // Q: is pointer assignment what I actually want to do here?
}
