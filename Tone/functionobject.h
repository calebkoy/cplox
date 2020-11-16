#ifndef FUNCTION_OBJECT_H
#define FUNCTION_OBJECT_H

#include "chunk.h"
#include "object.h"
#include "stringobject.h"

#include <memory> // TODO: remove if not using std::unique_ptr or std::shared_ptr

enum FunctionType {
  TYPE_FUNCTION,
  TYPE_INITIALIZER,
  TYPE_METHOD,
  TYPE_SCRIPT
};

class FunctionObject : public Object {
  int arity;
  int upvalueCount;
  Chunk chunk;
  //StringObject* name;
  std::shared_ptr<StringObject> name;

public:
  FunctionObject();

  void incrementArity();
  void incrementUpvalueCount();

  // TODO: depending on what callers of getName() expect, either return the shared_ptr or
  // return the resource. For now, it seems fine to return the resource.
  StringObject* getName();
  Chunk* getChunk();
  int getArity();
  int getUpvalueCount();

  //void setName(StringObject* name);
  void setName(std::shared_ptr<StringObject> name);

  friend std::ostream& operator<<(std::ostream& out, const FunctionObject &object) {
    return out << (object.name)->getChars(); // Q: parentheses necessary?
  }
};


#endif // FUNCTION_OBJECT_H
