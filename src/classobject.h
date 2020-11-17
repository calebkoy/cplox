#ifndef CLASS_OBJECT_H
#define CLASS_OBJECT_H

#include "object.h"
#include "stringobject.h"
#include "value.h"

#include <memory>
#include <unordered_map>

class ClassObject : public Object {
  std::shared_ptr<StringObject> name;
  std::unordered_map<std::string, Value> methods;

public:
  ClassObject(std::shared_ptr<StringObject> name);

  void setMethod(const std::string &name, const Value &method);
  Value getMethod(const std::string &name);
  bool findMethod(const std::string &name);

  std::shared_ptr<StringObject> getName();
  std::unordered_map<std::string, Value>* getMethods();
};

#endif // CLASS_OBJECT_H
