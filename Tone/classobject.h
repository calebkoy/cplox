#ifndef CLASS_OBJECT_H
#define CLASS_OBJECT_H

#include <unordered_map>

#include "object.h"
#include "stringobject.h"
#include "value.h"

// Q: is it possible to print this using an asClass() method in the Value class again?

class ClassObject : public Object {
  StringObject* name;
  std::unordered_map<std::string, Value> methods; // Q: should the values be pointers?

public:
  ClassObject(); // Q: do we need a default constructor? Will we allow a class w/ no name?
  ClassObject(StringObject* name);

  void setMethod(const std::string &name, Value method);

  StringObject* getName();
};

#endif // CLASS_OBJECT_H
