#include "compiler.h"
#include "scanner.h"
#include "tone.h"
#include "vm.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

void Tone::repl() {
  std::string line;

  // Q: does 'string interning' provide any value to Tone?
  // If not, remove the strings map.
  std::unordered_map<std::string, Value> strings;

  for (;;) {
    std::cout << ">> ";

    std::getline(std::cin, line);
    if (line.empty()) {
      std::cout << '\n';
      break;
    }
    interpret(line, &strings);
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
  if ((int)size == 0) return;
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

  // Q: does 'string interning' provide any value to Tone?
  // If not, remove the strings map.
  // Q: is it fine to leave this uninitialised?
  std::unordered_map<std::string, Value> strings;

  // Todo: refactor use of `strings` in both repl and runFile

  InterpretResult result = interpret(source, &strings);

  if (result == INTERPRET_COMPILATION_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

std::ostream& operator<<(std::ostream& out, TokenType type) {
  std::string s;
#define PROCESS_VAL(p) case(p): s = #p; break
  switch(type) {
    PROCESS_VAL(TOKEN_LEFT_PAREN);
    PROCESS_VAL(TOKEN_RIGHT_PAREN);
    PROCESS_VAL(TOKEN_LEFT_BRACE);
    PROCESS_VAL(TOKEN_RIGHT_BRACE);
    PROCESS_VAL(TOKEN_STAR);
    PROCESS_VAL(TOKEN_SLASH);
    PROCESS_VAL(TOKEN_PLUS);
    PROCESS_VAL(TOKEN_MINUS);
    PROCESS_VAL(TOKEN_SEMICOLON);
    PROCESS_VAL(TOKEN_DOT);
    PROCESS_VAL(TOKEN_COMMA);
    PROCESS_VAL(TOKEN_BANG);
    PROCESS_VAL(TOKEN_BANG_EQUAL);
    PROCESS_VAL(TOKEN_EQUAL);
    PROCESS_VAL(TOKEN_EQUAL_EQUAL);
    PROCESS_VAL(TOKEN_GREATER);
    PROCESS_VAL(TOKEN_GREATER_EQUAL);
    PROCESS_VAL(TOKEN_LESS);
    PROCESS_VAL(TOKEN_LESS_EQUAL);
    PROCESS_VAL(TOKEN_TRUE);
    PROCESS_VAL(TOKEN_FALSE);
    PROCESS_VAL(TOKEN_NULL);
    PROCESS_VAL(TOKEN_STRING);
    PROCESS_VAL(TOKEN_NUMBER);
    PROCESS_VAL(TOKEN_FUNCTION);
    PROCESS_VAL(TOKEN_CLASS);
    PROCESS_VAL(TOKEN_VAR);
    PROCESS_VAL(TOKEN_EXTENDS);
    PROCESS_VAL(TOKEN_PRINT); // Remove when print is in standard library
    PROCESS_VAL(TOKEN_FOR);
    PROCESS_VAL(TOKEN_WHILE);
    PROCESS_VAL(TOKEN_IF);
    PROCESS_VAL(TOKEN_ELSE);
    PROCESS_VAL(TOKEN_RETURN);
    PROCESS_VAL(TOKEN_OR);
    PROCESS_VAL(TOKEN_AND);
    PROCESS_VAL(TOKEN_THIS);
    PROCESS_VAL(TOKEN_SUPER);
    PROCESS_VAL(TOKEN_IDENTIFIER);
    PROCESS_VAL(TOKEN_EOF);
    PROCESS_VAL(TOKEN_ERROR);
  }
#undef PROCESS_VAL

  return out << s;
}

InterpretResult Tone::interpret(const std::string& source,
                                std::unordered_map<std::string, Value> *strings) {
  Scanner scanner{ source };
  std::vector<Token> tokens = scanner.scanTokens();

//  auto begin{ tokens.begin() };
//  auto end{ tokens.end() };
//  for (auto p{ begin }; p != end; ++p) {
//    Token token = *p;
//    std::cout << token.type << " " << source.substr(token.start, token.length) << '\n';
//  }

  //Chunk chunk;

  // Q: better way to pass objects pointer?
  // Q: better way to pass unordered map?
  //Compiler compiler{ tokens, &chunk, objects, strings };
  Compiler compiler{ tokens, strings };
  FunctionObject* function = compiler.compile();
  if (function == NULL) return INTERPRET_COMPILATION_ERROR;
//  if (!compiler.compile()) {
//    return INTERPRET_COMPILATION_ERROR;
//  }

  //vm.setChunk(chunk);
  //vm.resetProgramCounter(); // Q: do I need an equivalent version to reset the function program counters?

  // Q: would it be more OOP if VM did the operations below?
  vm.getStack()->push(Value{ function }); // Related to garbage collection. Q: is it necessary?

  ClosureObject* closure = new ClosureObject{ function }; // Q: how to avoid memory leaks?

  // Related to garbage collection. Q: is it necessary?
  vm.getStack()->pop();
  vm.getStack()->push(Value{ closure });

  CallFrame frame = { closure, 0, vm.getStack()->getTop() - 1 };
  vm.getCallFrames()[vm.getCallFrameCount()] = frame;
  vm.incrementCallFrameCount();

  return vm.run();
}
