#include "cplox.h"

#include <iostream>

int main(int argc, char *argv[]) {
  Cplox cplox;

  if (argc == 1) {
    cplox.repl();
  } else if (argc == 2) {
    cplox.runFile(argv[1]);
  } else {
    std::cerr << "Expected usage: cplox [path]\n";
    exit(64);
  }

  return 0;
}
