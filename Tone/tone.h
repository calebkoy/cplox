#ifndef TONE_H
#define TONE_H

#include "scanner.h"
#include "vm.h"

#include <string>
#include <vector>

class Tone {
private:
  Scanner scanner;
  VM vm;
  InterpretResult interpret(const std::string &source);

public:
  void repl();
  void runFile(const char *path);
};

#endif // TONE_H
