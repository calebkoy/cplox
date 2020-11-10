#ifndef TONE_H
#define TONE_H

#include "scanner.h"
#include "vm.h"

#include <string>
#include <vector>

class Tone {
private:
  VM vm;
  InterpretResult interpret(const std::string& source,
                            std::unordered_map<std::string, Value> *strings);

public:
  void repl();
  void runFile(const char *path);
};

#endif // TONE_H
