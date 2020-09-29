#ifndef TONE_H
#define TONE_H

#include <string>
#include <vector>

#include "scanner.h"
#include "vm.h"

class Tone {
private:
  // Q: should this be initialised when Tone is constructed?
  // Perhaps VM should have an explicit default constructor.
  VM vm;

  InterpretResult interpret(const std::string& source, Object *&objects,
                            std::unordered_map<std::string, Value> *strings);

public:
  void repl();
  void runFile(const char *path);
};

#endif // TONE_H
