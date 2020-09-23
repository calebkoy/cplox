#ifndef SCANNER_H
#define SCANNER_H

#include <map>
#include <string>
#include <vector>

#include "error_reporter.h"

enum TokenType {
  // Single-character tokens
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_STAR,       TOKEN_SLASH,
  TOKEN_PLUS,       TOKEN_MINUS,
  TOKEN_SEMICOLON,
  TOKEN_DOT,
  TOKEN_COMMA,

  // Tokens which could have one or two characters
  TOKEN_BANG,       TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,      TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,    TOKEN_GREATER_EQUAL,
  TOKEN_LESS,       TOKEN_LESS_EQUAL,

  // Literals
  TOKEN_TRUE,       TOKEN_FALSE,
  TOKEN_NULL,
  TOKEN_STRING,
  TOKEN_NUMBER,

  // Keywords
  TOKEN_FUNCTION,
  TOKEN_CLASS,
  TOKEN_VAR,
  TOKEN_EXTENDS,
  TOKEN_PRINT, // Remove once print() library function has been implemented
  TOKEN_FOR,
  TOKEN_WHILE,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_RETURN,
  TOKEN_OR,
  TOKEN_AND,
  TOKEN_THIS,
  TOKEN_SUPER,

  // Other
  TOKEN_IDENTIFIER,
  TOKEN_EOF
};

typedef struct {
  TokenType type;
  int line;
  int length;
} Token;

class Scanner {
  std::string source;
  std::vector<Token> tokens;
  int start{ 0 };
  int current{ 0 };
  int line{ 1 };
  ErrorReporter reporter;
  std::unordered_map<std::string, TokenType> reservedWords;

public:
  Scanner(const std::string source);

  std::vector<Token> scanTokens();

private:
  bool isAtEnd();
  void scanToken();
  char advance();
  bool isDigit(char c);
  bool isAlpha(char c);
  void addToken(TokenType type);
  void addToken(TokenType type, int length);
  Token makeToken(TokenType type);
  bool match(char c);
  char peek();
  char peekNext();
  void scanString();
  void scanNumber();
  void scanAlpha();
};

#endif // SCANNER_H
