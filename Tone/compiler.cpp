#include <iostream>

#include "compiler.h"

void Compiler::compile(const std::string &source, const std::vector<Token> &tokens) {
  int line = -1;
  auto begin{ tokens.cbegin() };
  auto end{ tokens.cend() };
  for(auto p{ begin }; p != end; ++p) {
    Token token = *p;
    if (token.line != line) {
      std::cout << token.line;
      line = token.line;
    } else {
      std::cout << "   | ";
    }
    std::string text = source.substr(token.start, token.length);
    std::cout << token.type << " '" << text << "'\n";
  }
}
