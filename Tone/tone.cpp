#include "compiler.h"
#include "scanner.h"
#include "tone.h"
#include "vm.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>

void Tone::repl() {
  std::string line;
  for (;;) {
    std::cout << ">> ";
    std::getline(std::cin, line);
    if (line.empty()) {
      std::cout << '\n';
      break;
    }
    interpret(line);
  }
}

void Tone::runFile(const char *path) {
  std::ifstream file{ path, std::ios::binary };
  if (!file) {
    std::cerr << "Cannot open " << path << " for reading.\n";
    exit(66);
  }

  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  if (static_cast<int>(size) == 0) return;
  std::string source(size, ' ');
  file.seekg(0);
  std::size_t  read = 0;
  do
  {
    file.read(&source[read], size - read);
    std::size_t amount = file.gcount();
    if (amount == 0)
    {
      std::cerr << "Encountered an error when reading the file at path: " << path << '\n';
      exit(74);
    }
    read += amount;
  } while(size != read);

  InterpretResult result = interpret(source);
  // TODO: ensure all memory has been cleaned up before exiting.
  if (result == INTERPRET_COMPILATION_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

InterpretResult Tone::interpret(const std::string &source) {
  scanner.reset();
  scanner.setSource(source);
  std::vector<Token> tokens = scanner.scanTokens();

  Compiler compiler{ tokens, &strings };
  FunctionObject* function = compiler.compile();
  if (function == nullptr) return INTERPRET_COMPILATION_ERROR;

  auto closure{ std::make_unique<ClosureObject>(function) };
  vm.pushOntoStack(Value{ closure.get() });
  vm.call(closure.get(), 0);
  return vm.run();
}
