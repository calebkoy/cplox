#include <iostream>

#include "compiler.h"

#define DEBUG_PRINT_CODE

// Q: should I be using smart pointers and/or move semantics here
// to ensure that there are no memory leaks?
// See https://stackoverflow.com/questions/7575459/c-should-i-initialize-pointer-members-that-are-assigned-to-in-the-constructor
Compiler::Compiler(const std::vector<Token> tokens, Chunk *chunk) :
  tokens{ tokens }, chunk{ chunk }, compilingChunk{ chunk } {
}

bool Compiler::compile() {
  advance();
  expression();
  consume(TOKEN_EOF, "Expect EOF token at end of expression.");
  endCompiler();
  return !hadError;
}

void Compiler::advance() {
  previous = current;

  for (;;) {
    current = tokens.at(currentTokenIndex++);
    if (current.type != TOKEN_ERROR) break;

    errorAtCurrent(current.lexeme);
  }
}

void Compiler::expression() {
  parsePrecedence(PRECEDENCE_ASSIGNMENT);
}

void Compiler::parsePrecedence(Precedence precedence) {
  advance();
  invokePrefixRule();
  while (precedence <= tokenPrecedence[current.type]) {
    advance();
    invokeInfixRule();
  }
}

void Compiler::invokePrefixRule() {
  // Q: should I be throwing errors instead of breaking if
  // switch matches an 'unexpected' token (but not via the default case)?
  switch (previous.type) {
    case TOKEN_LEFT_PAREN: grouping(); break;
    case TOKEN_RIGHT_PAREN: break;
    case TOKEN_LEFT_BRACE: break;
    case TOKEN_RIGHT_BRACE: break;
    case TOKEN_COMMA: break;
    case TOKEN_DOT: break;
    case TOKEN_MINUS: unary(); break;
    case TOKEN_PLUS: break;
    case TOKEN_SEMICOLON: break;
    case TOKEN_SLASH: break;
    case TOKEN_STAR: break;
    case TOKEN_BANG: break;
    case TOKEN_BANG_EQUAL: break;
    case TOKEN_EQUAL: break;
    case TOKEN_EQUAL_EQUAL: break;
    case TOKEN_GREATER: break;
    case TOKEN_GREATER_EQUAL: break;
    case TOKEN_LESS: break;
    case TOKEN_LESS_EQUAL: break;
    case TOKEN_IDENTIFIER: break;
    case TOKEN_STRING: break;
    case TOKEN_NUMBER: number(); break;
    case TOKEN_AND: break;
    case TOKEN_CLASS: break;
    case TOKEN_ELSE: break;
    case TOKEN_FALSE: literal(); break;
    case TOKEN_FOR: break;
    case TOKEN_FUNCTION: break;
    case TOKEN_IF: break;
    case TOKEN_NULL: literal(); break;
    case TOKEN_OR: break;
    case TOKEN_PRINT: break;
    case TOKEN_RETURN: break;
    case TOKEN_SUPER: break;
    case TOKEN_THIS: break;
    case TOKEN_TRUE: literal(); break;
    case TOKEN_VAR: break;
    case TOKEN_WHILE: break;
    case TOKEN_ERROR: break;
    case TOKEN_EOF: break;
    default: break;
  }
}

// Q: could this be refactored to use a map or other data structure that stores functions?
void Compiler::invokeInfixRule() {
  switch (previous.type) {
    case TOKEN_MINUS:
    case TOKEN_PLUS:
    case TOKEN_SLASH:
    case TOKEN_STAR: binary();
                     break;

    case TOKEN_LEFT_PAREN:
    case TOKEN_RIGHT_PAREN:
    case TOKEN_LEFT_BRACE:
    case TOKEN_RIGHT_BRACE:
    case TOKEN_COMMA:
    case TOKEN_DOT:
    case TOKEN_SEMICOLON:
    case TOKEN_BANG:
    case TOKEN_BANG_EQUAL:
    case TOKEN_EQUAL:
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
    case TOKEN_NUMBER:
    case TOKEN_AND:
    case TOKEN_CLASS:
    case TOKEN_ELSE:
    case TOKEN_FALSE:
    case TOKEN_FOR:
    case TOKEN_FUNCTION:
    case TOKEN_IF:
    case TOKEN_NULL:
    case TOKEN_OR:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
    case TOKEN_SUPER:
    case TOKEN_THIS:
    case TOKEN_TRUE:
    case TOKEN_VAR:
    case TOKEN_WHILE:
    case TOKEN_ERROR:
    case TOKEN_EOF:
    default:
      break;
  }
}

void Compiler::number() {
  // Q: is there a better way to do this?
  // According to this thread, it depends on the locale:
  // https://stackoverflow.com/questions/1012571/stdstring-to-float-or-double
  double value = std::stod(previous.lexeme);
  emitConstant(Value{ VAL_NUMBER, value });

}

void Compiler::literal() {
  switch (previous.type) {
    case TOKEN_FALSE: emitByte(OP_FALSE); break;
    case TOKEN_NULL: emitByte(OP_NULL); break;
    case TOKEN_TRUE: emitByte(OP_TRUE); break;
    default:
      return;
  }
}

// Q: should this class be passed by value?
void Compiler::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t Compiler::makeConstant(Value value) {
  int constant = (*compilingChunk).addConstant(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0; // Q: could this be problematic?
  }

  return (uint8_t)constant;
}

void Compiler::consume(TokenType type, const std::string &message) {
  if (type == TOKEN_EOF && current.type == type) return;

  if (current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

void Compiler::grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary() {
  TokenType operatorType = previous.type;
  parsePrecedence(PRECEDENCE_UNARY);
  switch (operatorType) {
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default:
      return;
  }
}

void Compiler::binary() {
  TokenType operatorType = previous.type;
  Precedence precedence = tokenPrecedence[operatorType];
  parsePrecedence((Precedence)(precedence + 1));
  switch (operatorType) {
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    default:
      return;
  }
}

void Compiler::emitByte(uint8_t byte) {
  (*compilingChunk).appendByte(byte, previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

void Compiler::endCompiler() {
  emitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!hadError) (*compilingChunk).disassemble();
#endif // DEBUG_PRINT_CODE
}

void Compiler::emitReturn() {
  emitByte(OP_RETURN);
}

void Compiler::error(const std::string &message) {
  errorAt(previous, message);
}

void Compiler::errorAtCurrent(const std::string &message) {
  errorAt(current, message);
}

void Compiler::errorAt(Token token, const std::string &message) {
  if (panicMode) return;
  panicMode = true;

  std::cerr << "[line " << token.line << "] Error";

  if (token.type == TOKEN_EOF) {
    std::cerr << " at end";
  } else if (token.type == TOKEN_ERROR) {
    // Nothing. Sure?
  } else {
    std::cerr << " at " << token.lexeme;
  }

  std::cerr << ": " << message << '\n';
  hadError = true;
}
