#ifndef TONE_H
#define TONE_H

#include <string>
#include <vector>

#include "scanner.h"

class Tone {
private:
  void interpret(const std::string& source);

public:
  void repl();
  void runFile(const char *path);
};

#endif // TONE_H
