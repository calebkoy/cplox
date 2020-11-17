#include "stringobject.h"

StringObject::StringObject(const std::string &chars) :
  Object{ ObjectType::OBJECT_STRING }, chars{ chars } {
}

std::string StringObject::getChars() {
  return chars;
}
