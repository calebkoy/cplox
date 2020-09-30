#ifndef NATIVE_OBJECT_H
#define NATIVE_OBJECT_H

#include "object.h"
#include "value.h"

typedef Value (*nativeFunctionPointer)(int argCount, Value* args);

class NativeObject : public Object {
  nativeFunctionPointer function;

public:
  NativeObject();
  NativeObject(nativeFunctionPointer function);

  nativeFunctionPointer getFunction();
};

#endif // NATIVE_OBJECT_H
