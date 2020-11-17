#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <unordered_map>
#include <vector>

enum class TokenType {
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
  TOKEN_PRINT,
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
  TOKEN_EOF,
  TOKEN_ERROR
};

struct Token {
  TokenType type{ TokenType::TOKEN_NULL };
  int line{ 0 };
  int length{ 0 };
  std::string lexeme;
};

class Scanner {
  std::string source;
  std::vector<Token> tokens;
  int start{ 0 };
  int current{ 0 };
  int line{ 1 };
  std::unordered_map<std::string, TokenType> reservedWords = {
    {"true", TokenType::TOKEN_TRUE},
    {"false", TokenType::TOKEN_FALSE},
    {"null", TokenType::TOKEN_NULL},
    {"function", TokenType::TOKEN_FUNCTION},
    {"class", TokenType::TOKEN_CLASS},
    {"var", TokenType::TOKEN_VAR},
    {"extends", TokenType::TOKEN_EXTENDS},
    {"print", TokenType::TOKEN_PRINT},
    {"for", TokenType::TOKEN_FOR},
    {"while", TokenType::TOKEN_WHILE},
    {"if", TokenType::TOKEN_IF},
    {"else", TokenType::TOKEN_ELSE},
    {"return", TokenType::TOKEN_RETURN},
    {"or", TokenType::TOKEN_OR},
    {"and", TokenType::TOKEN_AND},
    {"this", TokenType::TOKEN_THIS},
    {"super", TokenType::TOKEN_SUPER}
  };

  bool isAtEnd();
  void scanToken();
  char advance();
  bool isDigit(char c);
  bool isAlpha(char c);
  void addToken(TokenType type, const std::string &lexeme, int length);
  bool match(char c);
  char peek();
  char peekNext();
  void scanString();
  void scanNumber();
  void scanAlpha();

public:
  std::vector<Token> scanTokens();
  void reset();
  void setSource(const std::string &source);
};

#endif // SCANNER_H
