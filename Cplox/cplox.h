#ifndef CPLOX_H
#define CPLOX_H

#include "compiler.h"
#include "scanner.h"
#include "vm.h"

#include <string>
#include <unordered_map>
#include <vector>

class Cplox {
private:
  Scanner scanner;
  Compiler compiler;
  VM vm;
  std::unordered_map<std::string, Value> strings;

  InterpreterResult interpret(const std::string &source);

public:
  Cplox();

  void repl();
  void runFile(const char *path);
};

#endif // CPLOX_H

