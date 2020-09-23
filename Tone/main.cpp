#include <cstdio>
#include <iostream>

void repl() {
  char line[1024];
  for (;;) {
    std::cout << ">> ";

    if (!fgets(line, sizeof(line), stdin)) {
      std::cout << '\n';
      break;
    }

    std::cout << line << '\n';
  }
}

void runFile() {
  std::cout << "runFile()\n"; // Temp
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile();
  } else {
    // Print an error, telling the user the expected usage
    // Exit w/ an exit code
  }

  return 0;
}
