#ifndef TONE_H
#define TONE_H

#include "scanner.h"
#include "vm.h"

#include <string>
#include <unordered_map>
#include <vector>

class Tone {
private:
  Scanner scanner;
  VM vm;
  std::unordered_map<std::string, Value> strings;

  InterpretResult interpret(const std::string &source);

public:
  void repl();
  void runFile(const char *path);
};

#endif // TONE_H
