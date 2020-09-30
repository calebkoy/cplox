#ifndef FUNCTION_OBJECT_H
#define FUNCTION_OBJECT_H

#include "chunk.h"
#include "object.h"
#include "stringobject.h"

enum FunctionType {
  TYPE_FUNCTION,
  TYPE_SCRIPT
};

class FunctionObject : public Object {
  int arity;
  Chunk chunk;
  StringObject* name;

public:
  FunctionObject();

  StringObject* getName();
  Chunk* getChunk();

  friend std::ostream& operator<<(std::ostream& out, const FunctionObject &object) {
    return out << (object.name)->getChars(); // Q: parentheses necessary?
  }
};


#endif // FUNCTION_OBJECT_H
