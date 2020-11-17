#include "cplox.h"

#include <fstream>
#include <iostream>
#include <memory>

Cplox::Cplox() : compiler{ &strings }
{
}

void Cplox::repl() {
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

void Cplox::runFile(const char *path) {
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
  if (result == INTERPRET_COMPILATION_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

InterpretResult Cplox::interpret(const std::string &source) {
  scanner.reset();
  scanner.setSource(source);
  std::vector<Token> tokens = scanner.scanTokens();

  compiler.reset();
  compiler.setTokens(tokens);
  auto function = compiler.compile();
  if (function == nullptr) return INTERPRET_COMPILATION_ERROR;

  auto closure{ std::make_shared<ClosureObject>(function) };
  vm.pushOntoStack(Value{ closure });
  vm.call(closure.get(), 0);
  return vm.run();
}

