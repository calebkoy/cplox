#ifndef CLASS_OBJECT_H
#define CLASS_OBJECT_H

#include "object.h"
#include "stringobject.h"
#include "value.h"

#include <memory>
#include <unordered_map>

// Q: is it possible to print this using an asClass() method in the Value class again?

class ClassObject : public Object {
  //StringObject* name;
  std::shared_ptr<StringObject> name;
  std::unordered_map<std::string, Value> methods; // Q: should the values be pointers?

public:
  ClassObject(); // Q: do we need a default constructor? Will we allow a class w/ no name?
  ClassObject(std::shared_ptr<StringObject> name);

  void setMethod(const std::string &name, Value method);
  Value getMethod(const std::string &name);
  bool findMethod(const std::string &name);

  std::shared_ptr<StringObject> getName();
  std::unordered_map<std::string, Value>* getMethods();
};

#endif // CLASS_OBJECT_H
