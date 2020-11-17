#include "scanner.h"

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  addToken(TokenType::TOKEN_EOF, "", 0);

  return tokens;
}

bool Scanner::isAtEnd() {
  return (current >= 0 && static_cast<size_t>(current) >= source.length());
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
    case '(':
      addToken(TokenType::TOKEN_LEFT_PAREN, source.substr(start, current-start), 1);
      break;
    case ')':
      addToken(TokenType::TOKEN_RIGHT_PAREN, source.substr(start, current-start), 1);
      break;
    case '{':
      addToken(TokenType::TOKEN_LEFT_BRACE, source.substr(start, current-start), 1);
      break;
    case '}':
      addToken(TokenType::TOKEN_RIGHT_BRACE, source.substr(start, current-start), 1);
      break;
    case '+':
      addToken(TokenType::TOKEN_PLUS, source.substr(start, current-start), 1);
      break;
    case '-':
      addToken(TokenType::TOKEN_MINUS, source.substr(start, current-start), 1);
      break;
    case '*':
      addToken(TokenType::TOKEN_STAR, source.substr(start, current-start), 1);
      break;
    case ';':
      addToken(TokenType::TOKEN_SEMICOLON, source.substr(start, current-start), 1);
      break;
    case '.':
      addToken(TokenType::TOKEN_DOT, source.substr(start, current-start), 1);
      break;
    case ',':
      addToken(TokenType::TOKEN_COMMA, source.substr(start, current-start), 1);
      break;
    case '!':
      match('=') ?
        addToken(TokenType::TOKEN_BANG_EQUAL, source.substr(start, current-start), 2) :
        addToken(TokenType::TOKEN_BANG, source.substr(start, current-start), 1);
      break;
    case '=':
      match('=') ?
        addToken(TokenType::TOKEN_EQUAL_EQUAL, source.substr(start, current-start), 2) :
        addToken(TokenType::TOKEN_EQUAL, source.substr(start, current-start), 1);
      break;
    case '>':
      match('=') ?
        addToken(TokenType::TOKEN_GREATER_EQUAL, source.substr(start, current-start), 2) :
        addToken(TokenType::TOKEN_GREATER, source.substr(start, current-start), 1);
      break;
    case '<':
      match('=') ?
        addToken(TokenType::TOKEN_LESS_EQUAL, source.substr(start, current-start), 2) :
        addToken(TokenType::TOKEN_LESS, source.substr(start, current-start), 1);
      break;
    case '/': {
      if (match('/')) {
        while (!isAtEnd() && peek() != '\n') advance();
      } else {
        addToken(TokenType::TOKEN_SLASH, source.substr(start, current-start), 1);
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
      addToken(TokenType::TOKEN_ERROR, "Unexpected character.", 0);
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

  addToken(TokenType::TOKEN_NUMBER, source.substr(start, current-start), current - start);
}

void Scanner::scanAlpha() {
  while (isDigit(peek()) | isAlpha(peek())) advance();

  std::string text = source.substr(start, current-start);

  if (reservedWords.find(text) != reservedWords.end()) {
    addToken(reservedWords[text], text, current - start);
  } else {
    addToken(TokenType::TOKEN_IDENTIFIER, text, current - start);
  }
}

void Scanner::addToken(TokenType type, const std::string &lexeme, int length) {
  Token token;
  token.type = type;
  token.line = line;
  token.lexeme = lexeme;
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
  if (current + 1 >= 0 && static_cast<size_t>(current + 1) >= source.length()) return '\0';

  return source[current + 1];
}

void Scanner::scanString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }

  if (isAtEnd()) {
    addToken(TokenType::TOKEN_ERROR, "Unterminated string.", 0);
    return;
  }

  advance();
  int length = current - start - 2;
  std::string lexeme = source.substr(start+1, length);
  addToken(TokenType::TOKEN_STRING, lexeme, length);
}

void Scanner::reset() {
  tokens.clear();
  start = 0;
  current = 0;
  line = 1;
  source = "";
}

void Scanner::setSource(const std::string &source) {
  this->source = source;
}
