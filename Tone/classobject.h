#ifndef CLASS_OBJECT_H
#define CLASS_OBJECT_H

#include "object.h"
#include "stringobject.h"

class ClassObject : public Object {
  StringObject* name;

public:
  ClassObject(); // Q: do we need a default constructor? Will we allow a class w/ no name?
  ClassObject(StringObject* name);

  StringObject* getName();
};

#endif // CLASS_OBJECT_H
