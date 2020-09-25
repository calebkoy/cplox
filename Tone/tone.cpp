#include <fstream>
#include <iostream>

#include "compiler.h"
#include "scanner.h"
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

  // std::cout << source << '\n'; // Temp
  interpret(source);
  // If there's been a compilation or runtime error, exit w/ appropriate exit code
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
  }
#undef PROCESS_VAL

  return out << s;
}

void Tone::interpret(const std::string& source) {
  Scanner scanner{ source };
  std::vector<Token> tokens = scanner.scanTokens();

//  auto begin{ tokens.begin() };
//  auto end{ tokens.end() };
//  for (auto p{ begin }; p != end; ++p) {
//    Token token = *p;
//    std::cout << token.type << " " << source.substr(token.start, token.length) << '\n';
//  }

  Compiler compiler;
  compiler.compile(source, tokens);
}
