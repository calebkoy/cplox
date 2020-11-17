#ifndef OBJECT_H
#define OBJECT_H

#include <string>

enum class ObjectType {
  OBJECT_BOUND_METHOD,
  OBJECT_CLASS,
  OBJECT_CLOSURE,
  OBJECT_FUNCTION,
  OBJECT_INSTANCE,
  OBJECT_STRING,
  OBJECT_UPVALUE
};

class Object {
protected:
  ObjectType type;

public:
  Object(ObjectType type);

  ObjectType getType();

  friend std::ostream& operator<<(std::ostream& out, const Object &object);
};

#endif
