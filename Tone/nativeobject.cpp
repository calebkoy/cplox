#include "nativeobject.h"

NativeObject::NativeObject() : Object{ OBJECT_NATIVE } {
  function = nullptr;
}

NativeObject::NativeObject(nativeFunctionPointer function) : Object{ OBJECT_NATIVE } {
  this->function = function; // Q: is there a better way to assign function?
}

nativeFunctionPointer NativeObject::getFunction() {
  return function;
}
