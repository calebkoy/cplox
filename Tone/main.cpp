#include <fstream>
#include <iostream>
#include <string>

void repl() {
  std::string line;
  for (;;) {
    std::cout << ">> ";

    std::getline(std::cin, line);
    if (line.empty()) {
      std::cout << '\n';
      break;
    }

    std::cout << line << '\n'; // Temp
    // interpret(line);
  }
}

void runFile(const char *path) {
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
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    std::cerr << "Expected usage: tone [path]\n";
    exit(64);
  }

  return 0;
}
