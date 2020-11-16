#ifndef OBJECT_H
#define OBJECT_H

#include <string>

enum ObjectType {
  OBJECT_BOUND_METHOD,
  OBJECT_CLASS,
  OBJECT_CLOSURE,
  OBJECT_FUNCTION,
  OBJECT_INSTANCE,
  OBJECT_STRING,
  OBJECT_UPVALUE
};

// Q: should this class be thought of as abstract?
class Object {
protected:
  ObjectType type;
  Object* next;

public:
  Object(ObjectType type);

  // Q: should this be in this class?
  // Q: Should this be public?
  ObjectType getType();

  // Q: is there a better way to pass a pointer?
  // Q: how can I enforce that this is always overridden by classes that inherit from Object?
  // Q: does it even need to be overridden?
  // Q: should this be virtual? Is it ever called on an instance of Object? It's called on
  // instances of UpvalueObject
  void setNext(Object* next);

  // Q: same questions for setNext()
  Object* getNext();

  friend std::ostream& operator<<(std::ostream& out, const Object &object);
};

#endif
