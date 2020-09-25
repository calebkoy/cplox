#ifndef COMPILER_H
#define COMPILER_H

#include <vector>

#include "scanner.h"

class Compiler {
public:
  void compile(const std::string &source, const std::vector<Token> &tokens);
};

#endif // COMPILER_H
