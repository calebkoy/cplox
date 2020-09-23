#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

void repl() {
  char line[1024]; // Q: Could I use std::string?
  for (;;) {
    std::cout << ">> ";

    if (!fgets(line, sizeof(line), stdin)) {
      std::cout << '\n';
      break;
    }

    std::cout << line << '\n';
  }
}

void runFile(const char *path) {
  std::ifstream inputFile{ path };

  if (!inputFile) {
    std::cerr << "Cannot open " << path << " for reading.\n";
    exit(66);
  }

  while (inputFile) {
    std::string fileText;
    std::getline(inputFile, fileText);
    std::cout << fileText << '\n';
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    // Print an error, telling the user the expected usage
    // Exit w/ an exit code
  }

  return 0;
}
