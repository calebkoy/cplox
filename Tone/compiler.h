#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "scanner.h"

enum Precedence {
  PRECEDENCE_NONE,
  PRECEDENCE_ASSIGNMENT,
  PRECEDENCE_OR,
  PRECEDENCE_AND,
  PRECEDENCE_EQUALITY,    // == !=
  PRECEDENCE_COMPARISON,  // < > <= >=
  PRECEDENCE_TERM,        // + -
  PRECEDENCE_FACTOR,      // * /
  PRECEDENCE_UNARY,       // ! -
  PRECEDENCE_CALL,        // . ()
  PRECEDENCE_PRIMARY
};

typedef void (*ParseFn)();

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};

class Compiler {
  const std::vector<Token> tokens;
  Chunk* chunk;
  Chunk* compilingChunk;
  Token current;
  Token previous;
  int currentTokenIndex{ 0 };
  bool hadError{ false };
  bool panicMode{ false };
  std::unordered_map<TokenType, Precedence> tokenPrecedence = {
    {TOKEN_LEFT_PAREN, PRECEDENCE_NONE},
    {TOKEN_RIGHT_PAREN, PRECEDENCE_NONE},
    {TOKEN_LEFT_BRACE, PRECEDENCE_NONE},
    {TOKEN_RIGHT_BRACE, PRECEDENCE_NONE},
    {TOKEN_COMMA, PRECEDENCE_NONE},
    {TOKEN_DOT, PRECEDENCE_NONE},
    {TOKEN_MINUS, PRECEDENCE_TERM},
    {TOKEN_PLUS, PRECEDENCE_TERM},
    {TOKEN_SEMICOLON, PRECEDENCE_NONE},
    {TOKEN_SLASH, PRECEDENCE_FACTOR},
    {TOKEN_STAR, PRECEDENCE_FACTOR},
    {TOKEN_BANG, PRECEDENCE_NONE},
    {TOKEN_BANG_EQUAL, PRECEDENCE_NONE},
    {TOKEN_EQUAL, PRECEDENCE_NONE},
    {TOKEN_EQUAL_EQUAL, PRECEDENCE_NONE},
    {TOKEN_GREATER, PRECEDENCE_NONE},
    {TOKEN_GREATER_EQUAL, PRECEDENCE_NONE},
    {TOKEN_LESS, PRECEDENCE_NONE},
    {TOKEN_LESS_EQUAL, PRECEDENCE_NONE},
    {TOKEN_IDENTIFIER, PRECEDENCE_NONE},
    {TOKEN_STRING, PRECEDENCE_NONE},
    {TOKEN_NUMBER, PRECEDENCE_NONE},
    {TOKEN_AND, PRECEDENCE_NONE},
    {TOKEN_CLASS, PRECEDENCE_NONE},
    {TOKEN_ELSE, PRECEDENCE_NONE},
    {TOKEN_FALSE, PRECEDENCE_NONE},
    {TOKEN_FOR, PRECEDENCE_NONE},
    {TOKEN_FUNCTION, PRECEDENCE_NONE},
    {TOKEN_IF, PRECEDENCE_NONE},
    {TOKEN_NULL, PRECEDENCE_NONE},
    {TOKEN_OR, PRECEDENCE_NONE},
    {TOKEN_PRINT, PRECEDENCE_NONE},
    {TOKEN_RETURN, PRECEDENCE_NONE},
    {TOKEN_SUPER, PRECEDENCE_NONE},
    {TOKEN_THIS, PRECEDENCE_NONE},
    {TOKEN_TRUE, PRECEDENCE_NONE},
    {TOKEN_VAR, PRECEDENCE_NONE},
    {TOKEN_WHILE, PRECEDENCE_NONE},
    {TOKEN_ERROR, PRECEDENCE_NONE},
    {TOKEN_EOF, PRECEDENCE_NONE}
  };

  //ParseRule rules[100];

//  ParseRule rules[] = {
//    [TOKEN_LEFT_PAREN]    = { grouping, NULL,   PRECEDENCE_NONE },
//    [TOKEN_RIGHT_PAREN]   = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_LEFT_BRACE]    = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_RIGHT_BRACE]   = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_COMMA]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_DOT]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_MINUS]         = { unary,    binary, PRECEDENCE_TERM },
//    [TOKEN_PLUS]          = { NULL,     binary, PRECEDENCE_TERM },
//    [TOKEN_SEMICOLON]     = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_SLASH]         = { NULL,     binary, PRECEDENCE_FACTOR },
//    [TOKEN_STAR]          = { NULL,     binary, PRECEDENCE_FACTOR },
//    [TOKEN_BANG]          = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_BANG_EQUAL]    = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_EQUAL]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_EQUAL_EQUAL]   = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_GREATER]       = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_GREATER_EQUAL] = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_LESS]          = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_LESS_EQUAL]    = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_IDENTIFIER]    = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_STRING]        = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_NUMBER]        = { number,   NULL,   PRECEDENCE_NONE },
//    [TOKEN_AND]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_CLASS]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_ELSE]          = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_FALSE]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_FOR]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_FUNCTION]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_IF]            = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_NULL]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_OR]            = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_PRINT]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_RETURN]        = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_SUPER]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_THIS]          = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_TRUE]          = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_VAR]           = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_WHILE]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_ERROR]         = { NULL,     NULL,   PRECEDENCE_NONE },
//    [TOKEN_EOF]           = { NULL,     NULL,   PRECEDENCE_NONE }
//  };

  void advance();
  void consume(TokenType type, const std::string &message);
  void expression();
  void number();
  void grouping();
  void unary();
  void binary();
  void invokePrefixRule();
  void invokeInfixRule();
  void parsePrecedence(Precedence precedence);
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  void emitConstant(Value value);
  uint8_t makeConstant(Value value);
  void emitReturn();
  void endCompiler();
  void errorAtCurrent(const std::string &message);
  void error(const std::string &message);
  void errorAt(Token token, const std::string &message);
public:
  Compiler(const std::vector<Token> tokens, Chunk *chunk); // Q: how should Chunk be passed?

  bool compile();
};

#endif // COMPILER_H
