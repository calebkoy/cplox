#include "scanner.h"

Scanner::Scanner(const std::string source) {
  this->source = source;
}

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  addToken(TOKEN_EOF);

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
    case '(': addToken(TOKEN_LEFT_PAREN);                                 break;
    case ')': addToken(TOKEN_RIGHT_PAREN);                                break;
    case '{': addToken(TOKEN_LEFT_BRACE);                                 break;
    case '}': addToken(TOKEN_RIGHT_BRACE);                                break;
    case '+': addToken(TOKEN_PLUS);                                       break;
    case '-': addToken(TOKEN_MINUS);                                      break;
    case '*': addToken(TOKEN_STAR);                                       break;
    case ';': addToken(TOKEN_SEMICOLON);                                  break;
    case '.': addToken(TOKEN_DOT);                                        break;
    case ',': addToken(TOKEN_COMMA);                                      break;
    case '!': addToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);       break;
    case '=': addToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);     break;
    case '>': addToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER); break;
    case '<': addToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);       break;
    case '/': {
      if (match('/')) {
        while (!isAtEnd() && peek() != '\n') advance();
        if (peek() == '\n') line++;
      } else {
        addToken(TOKEN_SLASH);
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
      reporter.report("Unexpected character.");
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

  int length = current - start;
  addToken(TOKEN_NUMBER, length);
}

void Scanner::scanAlpha() {

}

void Scanner::addToken(TokenType type) {
  tokens.push_back(makeToken(type));
}

void Scanner::addToken(TokenType type, int length) {
  Token token = makeToken(type);
  token.length = length;
  tokens.push_back(token);
}

Token Scanner::makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.line = line;

  return token;
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
    reporter.report("Unterminated string");
    return;
  }

  advance();
  int length = current - start - 2;
  addToken(TOKEN_STRING, length);
}
