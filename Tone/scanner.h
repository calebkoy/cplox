#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

enum TokenType {
  TOKEN_LEFT_PAREN,
};

typedef struct {
  TokenType type;
  int line;
} Token;

class Scanner {
  std::string source;
  std::vector<Token> tokens;
  int start{ 0 };
  int current{ 0 };
  int line{ 1 };

public:
  Scanner(const std::string source);

  std::vector<Token> scanTokens();

private:
  bool isAtEnd();
  void scanToken();
  char advance();
  void addToken(TokenType type);
  Token makeToken(TokenType type);
};

#endif // SCANNER_H
