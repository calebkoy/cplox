#ifndef STRING_OBJECT_H
#define STRING_OBJECT_H

#include "object.h"

#include <string>

class StringObject : public Object {
  std::string chars;

public:
  StringObject(const std::string &chars);

  std::string getChars();

  friend std::ostream& operator<<(std::ostream& out, const StringObject &object) {
    return out << object.chars;
  }
};

#endif // STRING_OBJECT_H
