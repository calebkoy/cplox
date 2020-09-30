#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "environment.h"
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
  Environment currentEnvironment; // Q: should this be a pointer?
  Object* objects;
  std::unordered_map<std::string, Value> *strings;
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
    {TOKEN_BANG_EQUAL, PRECEDENCE_EQUALITY},
    {TOKEN_EQUAL, PRECEDENCE_NONE},
    {TOKEN_EQUAL_EQUAL, PRECEDENCE_EQUALITY},
    {TOKEN_GREATER, PRECEDENCE_COMPARISON},
    {TOKEN_GREATER_EQUAL, PRECEDENCE_COMPARISON},
    {TOKEN_LESS, PRECEDENCE_COMPARISON},
    {TOKEN_LESS_EQUAL, PRECEDENCE_COMPARISON},
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

  void advance();
  void consume(TokenType type, const std::string &message);
  void expression();
  void number();
  void grouping();
  void unary();
  void binary();
  void and_();
  void or_();
  void literal();
  void declaration();
  void functionDeclaration();
  void function(FunctionType type);
  void variable(bool canAssign);
  void namedVariable(Token name, bool canAssign);
  void varDeclaration();
  uint8_t parseVariable(const std::string &errorMessage);
  uint8_t identifierConstant(Token* name);
  void declareVariable();
  void defineVariable(uint8_t global);
  void markInitialised();
  void block();
  void beginScope();
  void endScope();
  void statement();
  void printStatement();
  void ifStatement();
  void whileStatement();
  void forStatement();
  Chunk* currentChunk();
  void emitLoop(int loopStart);
  int emitJump(uint8_t instruction);
  void patchJump(int offset);
  void expressionStatement();
  void synchronise();
  bool match(TokenType type);
  bool check(TokenType type);
  bool identifiersEqual(Token* a, Token* b);
  void string();
  StringObject* copyString(Token* name);
  int resolveLocal(Environment* environment, Token* name);
  void* reallocate(void* pointer, size_t oldSize, size_t newSize); // Q: Where's the best place for this function to reside?
  void invokePrefixRule(bool canAssign);
  void invokeInfixRule(bool canAssign);
  void parsePrecedence(Precedence precedence);
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  void emitConstant(Value value);
  uint8_t makeConstant(Value value);
  void emitReturn();
  FunctionObject* endCompiler();
  void errorAtCurrent(const std::string &message);
  void error(const std::string &message);
  void errorAt(Token token, const std::string &message);
public:
  // Q: how should objects be passed? Is reference fine, or should it be a double pointer?
  // See: https://stackoverflow.com/questions/10240161/reason-to-pass-a-pointer-by-reference-in-c
  // Q: better way to pass strings? Is it fine that caller doesn't know that it's being passed by reference?
  Compiler(const std::vector<Token> tokens,
           Object *&objects, std::unordered_map<std::string, Value> *strings);

  FunctionObject* compile();
};

#endif // COMPILER_H
