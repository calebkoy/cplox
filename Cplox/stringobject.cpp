#include "stringobject.h"

// Q: is there a better way to initialise a std::string?
StringObject::StringObject(const std::string &chars) : Object{ OBJECT_STRING }, chars{ chars } {
}

std::string StringObject::getChars() {
  return chars;
}
