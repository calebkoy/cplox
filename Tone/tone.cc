#include <fstream>
#include <iostream>

#include "tone.h"

void Tone::repl() {
  std::string line;
  for (;;) {
    std::cout << ">> ";

    std::getline(std::cin, line);
    if (line.empty()) {
      std::cout << '\n';
      break;
    }

//    std::cout << line << '\n'; // Temp
     interpret(line);
  }
}

void Tone::runFile(const char *path) {
  std::ifstream file{ path };

  if (!file) {
    std::cerr << "Cannot open " << path << " for reading.\n";
    exit(66);
  }

  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  std::string source(size, ' ');
  file.seekg(0);
  file.read(&source[0], size);

  std::cout << source << '\n'; // Temp
  // interpret(source);
  // If there's been a compilation or runtime error, exit w/ appropriate exit code
}

void Tone::interpret(const std::string& source) {
  Scanner scanner{ source };
  scanner.scanTokens();
}
