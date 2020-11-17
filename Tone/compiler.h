#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "environment.h"
#include "error_reporter.h"
#include "scanner.h"

#include <memory>
#include <unordered_map>
#include <vector>

enum class Precedence {
  NONE,
  ASSIGNMENT,
  OR,
  AND,
  EQUALITY,    // == !=
  COMPARISON,  // < > <= >=
  TERM,        // + -
  FACTOR,      // * /
  UNARY,       // ! -
  CALL,        // . ()
  PRIMARY
};

struct ClassEnvironment {
  struct ClassEnvironment* enclosing;
  Token name;
  bool hasSuperclass;
};

class Compiler {
  std::vector<Token> tokens;
  std::unordered_map<std::string, Value> *strings;
  std::unique_ptr<Environment> currentEnvironment;
  ClassEnvironment* currentClassEnvironment{ nullptr };
  Token current;
  Token previous;
  ErrorReporter reporter;
  int currentTokenIndex{ 0 };
  std::unordered_map<TokenType, Precedence> tokenPrecedence = {
    {TOKEN_LEFT_PAREN, Precedence::CALL},
    {TOKEN_RIGHT_PAREN, Precedence::NONE},
    {TOKEN_LEFT_BRACE, Precedence::NONE},
    {TOKEN_RIGHT_BRACE, Precedence::NONE},
    {TOKEN_COMMA, Precedence::NONE},
    {TOKEN_DOT, Precedence::CALL},
    {TOKEN_MINUS, Precedence::TERM},
    {TOKEN_PLUS, Precedence::TERM},
    {TOKEN_SEMICOLON, Precedence::NONE},
    {TOKEN_SLASH, Precedence::FACTOR},
    {TOKEN_STAR, Precedence::FACTOR},
    {TOKEN_BANG, Precedence::NONE},
    {TOKEN_BANG_EQUAL, Precedence::EQUALITY},
    {TOKEN_EQUAL, Precedence::NONE},
    {TOKEN_EQUAL_EQUAL, Precedence::EQUALITY},
    {TOKEN_GREATER, Precedence::COMPARISON},
    {TOKEN_GREATER_EQUAL, Precedence::COMPARISON},
    {TOKEN_LESS, Precedence::COMPARISON},
    {TOKEN_LESS_EQUAL, Precedence::COMPARISON},
    {TOKEN_IDENTIFIER, Precedence::NONE},
    {TOKEN_STRING, Precedence::NONE},
    {TOKEN_NUMBER, Precedence::NONE},
    {TOKEN_AND, Precedence::AND},
    {TOKEN_CLASS, Precedence::NONE},
    {TOKEN_ELSE, Precedence::NONE},
    {TOKEN_FALSE, Precedence::NONE},
    {TOKEN_FOR, Precedence::NONE},
    {TOKEN_FUNCTION, Precedence::NONE},
    {TOKEN_IF, Precedence::NONE},
    {TOKEN_NULL, Precedence::NONE},
    {TOKEN_OR, Precedence::OR},
    {TOKEN_PRINT, Precedence::NONE},
    {TOKEN_RETURN, Precedence::NONE},
    {TOKEN_SUPER, Precedence::NONE},
    {TOKEN_THIS, Precedence::NONE},
    {TOKEN_TRUE, Precedence::NONE},
    {TOKEN_VAR, Precedence::NONE},
    {TOKEN_WHILE, Precedence::NONE},
    {TOKEN_ERROR, Precedence::NONE},
    {TOKEN_EOF, Precedence::NONE}
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
  void this_();
  void super_();
  void call();
  void dot(bool canAssign);
  void method();
  uint8_t argumentList();
  void literal();
  void declaration();
  void functionDeclaration();
  void function(FunctionType type);
  void variable(bool canAssign);
  void namedVariable(const Token &name, bool canAssign);
  void varDeclaration();
  void classDeclaration();
  uint8_t parseVariable(const std::string &errorMessage);
  uint8_t identifierConstant(const Token &name);
  void declareVariable();
  void defineVariable(uint8_t global);
  Token syntheticToken(const std::string &text);
  void markInitialised();
  void block();
  void beginScope();
  void endScope();
  void statement();
  void printStatement();
  void ifStatement();
  void whileStatement();
  void forStatement();
  void returnStatement();
  Chunk* currentChunk();
  void emitLoop(int loopStart);
  int emitJump(uint8_t instruction);
  void patchJump(int offset);
  void expressionStatement();
  void synchronise();
  bool match(TokenType type);
  bool check(TokenType type);
  bool identifiersEqual(const Token &a, const Token &b);
  void string();
  std::shared_ptr<StringObject> copyString(const Token &name);
  void invokePrefixRule(bool canAssign);
  void invokeInfixRule(bool canAssign);
  void parsePrecedence(Precedence precedence);
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  void emitConstant(const Value &value);
  uint8_t makeConstant(const Value &value);
  void emitReturn();
  FunctionObject* endCompiler();

public:
  Compiler(std::unordered_map<std::string, Value> *strings);

  FunctionObject* compile();
  void reset();

  void setTokens(const std::vector<Token> &tokens);
};

#endif // COMPILER_H
