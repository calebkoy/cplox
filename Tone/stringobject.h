#ifndef STRING_OBJECT_H
#define STRING_OBJECT_H

#include <string>

#include "object.h"

class StringObject : public Object {
  std::string chars; // Q: better name than chars?

public:
  StringObject(const std::string &chars);

  std::string getChars(); // Q: is there a better way of returning the std::string?

  friend std::ostream& operator<<(std::ostream& out, const StringObject &object) {
    return out << object.chars;
  }
};

#endif // STRING_OBJECT_H
