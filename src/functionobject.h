#ifndef FUNCTION_OBJECT_H
#define FUNCTION_OBJECT_H

#include "chunk.h"
#include "object.h"
#include "stringobject.h"

#include <memory>

class FunctionObject : public Object {
  int arity{ 0 };
  int upvalueCount{ 0 };
  Chunk chunk;
  std::shared_ptr<StringObject> name;

public:
  enum class FunctionType {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
  };

  FunctionObject();

  void incrementArity();
  void incrementUpvalueCount();

  StringObject* getName();
  Chunk* getChunk();
  int getArity();
  int getUpvalueCount();
  void setName(std::shared_ptr<StringObject> name);

  friend std::ostream& operator<<(std::ostream& out, const FunctionObject &object) {
    return out << (object.name)->getChars();
  }
};


#endif // FUNCTION_OBJECT_H
