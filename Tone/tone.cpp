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

  // Q: would it be more OOP if VM did the operations below?

  // Wrap the FunctionObject* in a ClosureObject*
  // Wrap the ClosureObject* in a Value
  // Push the Value onto the VM's stack
  // Create a CallFrame that contains the closure and the Value on top of the VM's stack
  // and which has an instruction program counter of zero (it points to the first index
  // of the function's chunk's bytecode array)

  //ClosureObject* closure = new ClosureObject{ function }; // Q: how to avoid memory leaks?
  auto closure{ std::make_unique<ClosureObject>(function) };
  //auto closure{ std::make_shared<ClosureObject>(function) };

  // TODO: call a single function defined on VM to do this
  //vm.getStack()->push(Value{ std::move(closure) });
  vm.getStack()->push(Value{ closure.get() });
  //vm.getStack()->push(Value{ closure });

  CallFrame frame = { closure.get(), 0, vm.getStack()->getTop() - 1 };
  vm.getCallFrames()[vm.getCallFrameCount()] = frame;
  vm.incrementCallFrameCount();

  return vm.run();
}
