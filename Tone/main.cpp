#include <iostream>

#include "chunk.h"
#include "tone.h"
#include "vm.h"

int main(int argc, char *argv[]) {
  Tone tone;

  if (argc == 1) {
    tone.repl();
  } else if (argc == 2) {
    tone.runFile(argv[1]);
  } else {
    std::cerr << "Expected usage: tone [path]\n";
    exit(64);
  }

  return 0;
}
