#include "scanner.h"

Scanner::Scanner(const std::string source) {
  this->source = source;
}

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  addToken(TOKEN_EOF, "", (int)source.size(), 0);

  return tokens;
}

bool Scanner::isAtEnd() {
  return (current >= 0 && (size_t)current >= source.length());
}

void Scanner::scanToken() {
  char c = advance();

  if (isDigit(c)) {
    scanNumber();
    return;
  } else if (isAlpha(c)) {
    scanAlpha();
    return;
  }

  switch (c) {
    case '(': addToken(TOKEN_LEFT_PAREN, source.substr(start, current-start), start, 1);                                 break;
    case ')': addToken(TOKEN_RIGHT_PAREN, source.substr(start, current-start), start, 1);                                break;
    case '{': addToken(TOKEN_LEFT_BRACE, source.substr(start, current-start), start, 1);                                 break;
    case '}': addToken(TOKEN_RIGHT_BRACE, source.substr(start, current-start), start, 1);                                break;
    case '+': addToken(TOKEN_PLUS, source.substr(start, current-start), start, 1);                                       break;
    case '-': addToken(TOKEN_MINUS, source.substr(start, current-start), start, 1);                                      break;
    case '*': addToken(TOKEN_STAR, source.substr(start, current-start), start, 1);                                       break;
    case ';': addToken(TOKEN_SEMICOLON, source.substr(start, current-start), start, 1);                                  break;
    case '.': addToken(TOKEN_DOT, source.substr(start, current-start), start, 1);                                        break;
    case ',': addToken(TOKEN_COMMA, source.substr(start, current-start), start, 1);                                      break;
    case '!':
      match('=') ? addToken(TOKEN_BANG_EQUAL, source.substr(start, current-start), start, 2) : addToken(TOKEN_BANG, source.substr(start, current-start), start, 1);
      break;
    case '=':
      match('=') ? addToken(TOKEN_EQUAL_EQUAL, source.substr(start, current-start), start, 2) : addToken(TOKEN_EQUAL, source.substr(start, current-start), start, 1);
      break;
    case '>':
      match('=') ? addToken(TOKEN_GREATER_EQUAL, source.substr(start, current-start), start, 2) : addToken(TOKEN_GREATER, source.substr(start, current-start), start, 1);
      break;
    case '<':
      match('=') ? addToken(TOKEN_LESS_EQUAL, source.substr(start, current-start), start, 2) : addToken(TOKEN_LESS, source.substr(start, current-start), start, 1);
      break;
    case '/': {
      if (match('/')) {
        while (!isAtEnd() && peek() != '\n') advance();
        if (peek() == '\n') line++;
      } else {
        addToken(TOKEN_SLASH, source.substr(start, current-start), start, 1);
      }
      break;
    }
    case '"': {
      scanString();
    }
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line++;
      break;
    default:
      reporter.report(line, "Unexpected character.");
  }
}

char Scanner::advance() {
  current++;
  return source[current - 1];
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void Scanner::scanNumber() {
  while (isDigit(peek())) advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) advance();
  }

  addToken(TOKEN_NUMBER, source.substr(start, current-start), start, current - start);
}

void Scanner::scanAlpha() {
  while (isDigit(peek()) | isAlpha(peek())) advance();

  std::string text = source.substr(start, current-start);

  if (reservedWords.find(text) != reservedWords.end()) {
    addToken(reservedWords[text], text, start, current - start);
  } else {
    addToken(TOKEN_IDENTIFIER, text, start, current - start);
  }
}

void Scanner::addToken(TokenType type, const std::string &lexeme, int start, int length) {
  Token token;
  token.type = type;
  token.line = line;
  token.lexeme = lexeme;
  token.start = start;
  token.length = length;
  tokens.push_back(token);
}

bool Scanner::match(char c) {
  if (isAtEnd() || peek() != c) return false;
  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return source[current];
}

char Scanner::peekNext() {
  if (current + 1 >= 0 && (size_t)(current + 1) >= source.length()) return '\0';

  return source[current + 1];
}

void Scanner::scanString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }

  if (isAtEnd()) {
    reporter.report(line, "Unterminated string");
    return;
  }

  advance();
  int length = current - start - 2;
  std::string lexeme = source.substr(start+1, length);
  addToken(TOKEN_STRING, lexeme, start+1, length);
}
