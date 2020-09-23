#include "scanner.h"

Scanner::Scanner(const std::string source) {
  this->source = source;
}

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  return tokens;
}

bool Scanner::isAtEnd() {
  return (current >= 0 && (size_t)current >= source.length());
}

void Scanner::scanToken() {
  char character = advance();
  switch (character) {
    case '(':
      addToken(TOKEN_LEFT_PAREN);
      break;
  }
}

char Scanner::advance() {
  current++;
  return source[current - 1];
}

void Scanner::addToken(TokenType type) {
  tokens.push_back(makeToken(type));
}

Token Scanner::makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.line = line;

  return token;
}


