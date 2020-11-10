#include "compiler.h"
#include "scanner.h"
#include "tone.h"
#include "vm.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

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
  if (result == INTERPRET_COMPILATION_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

InterpretResult Tone::interpret(const std::string& source) {
  // TODO: instead of creating a scanner each time,
  // it might be better to give Tone a scanner
  Scanner scanner{ source };
  std::vector<Token> tokens = scanner.scanTokens();

  // Q: does 'string interning' provide any value to Tone?
  // If not, remove the strings map and the unordered_map header.
  // Q: is it fine to leave this uninitialised?
  std::unordered_map<std::string, Value> strings;

  // Q: better way to pass objects pointer?
  // Q: better way to pass unordered map?
  //Compiler compiler{ tokens, &chunk, objects, strings };
  Compiler compiler{ tokens, &strings };
  FunctionObject* function = compiler.compile();
  if (function == NULL) return INTERPRET_COMPILATION_ERROR;

  // Q: would it be more OOP if VM did the operations below?

  ClosureObject* closure = new ClosureObject{ function }; // Q: how to avoid memory leaks?
  vm.getStack()->push(Value{ closure });
  CallFrame frame = { closure, 0, vm.getStack()->getTop() - 1 };
  vm.getCallFrames()[vm.getCallFrameCount()] = frame;
  vm.incrementCallFrameCount();

  return vm.run();
}
