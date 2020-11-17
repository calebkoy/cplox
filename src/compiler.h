#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "environment.h"
#include "functionobject.h"
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
    {TokenType::TOKEN_LEFT_PAREN, Precedence::CALL},
    {TokenType::TOKEN_RIGHT_PAREN, Precedence::NONE},
    {TokenType::TOKEN_LEFT_BRACE, Precedence::NONE},
    {TokenType::TOKEN_RIGHT_BRACE, Precedence::NONE},
    {TokenType::TOKEN_COMMA, Precedence::NONE},
    {TokenType::TOKEN_DOT, Precedence::CALL},
    {TokenType::TOKEN_MINUS, Precedence::TERM},
    {TokenType::TOKEN_PLUS, Precedence::TERM},
    {TokenType::TOKEN_SEMICOLON, Precedence::NONE},
    {TokenType::TOKEN_SLASH, Precedence::FACTOR},
    {TokenType::TOKEN_STAR, Precedence::FACTOR},
    {TokenType::TOKEN_BANG, Precedence::NONE},
    {TokenType::TOKEN_BANG_EQUAL, Precedence::EQUALITY},
    {TokenType::TOKEN_EQUAL, Precedence::NONE},
    {TokenType::TOKEN_EQUAL_EQUAL, Precedence::EQUALITY},
    {TokenType::TOKEN_GREATER, Precedence::COMPARISON},
    {TokenType::TOKEN_GREATER_EQUAL, Precedence::COMPARISON},
    {TokenType::TOKEN_LESS, Precedence::COMPARISON},
    {TokenType::TOKEN_LESS_EQUAL, Precedence::COMPARISON},
    {TokenType::TOKEN_IDENTIFIER, Precedence::NONE},
    {TokenType::TOKEN_STRING, Precedence::NONE},
    {TokenType::TOKEN_NUMBER, Precedence::NONE},
    {TokenType::TOKEN_AND, Precedence::AND},
    {TokenType::TOKEN_CLASS, Precedence::NONE},
    {TokenType::TOKEN_ELSE, Precedence::NONE},
    {TokenType::TOKEN_FALSE, Precedence::NONE},
    {TokenType::TOKEN_FOR, Precedence::NONE},
    {TokenType::TOKEN_FUNCTION, Precedence::NONE},
    {TokenType::TOKEN_IF, Precedence::NONE},
    {TokenType::TOKEN_NULL, Precedence::NONE},
    {TokenType::TOKEN_OR, Precedence::OR},
    {TokenType::TOKEN_PRINT, Precedence::NONE},
    {TokenType::TOKEN_RETURN, Precedence::NONE},
    {TokenType::TOKEN_SUPER, Precedence::NONE},
    {TokenType::TOKEN_THIS, Precedence::NONE},
    {TokenType::TOKEN_TRUE, Precedence::NONE},
    {TokenType::TOKEN_VAR, Precedence::NONE},
    {TokenType::TOKEN_WHILE, Precedence::NONE},
    {TokenType::TOKEN_ERROR, Precedence::NONE},
    {TokenType::TOKEN_EOF, Precedence::NONE}
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
  void function(FunctionObject::FunctionType type);
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
  std::shared_ptr<FunctionObject> endCompiler();

public:
  Compiler(std::unordered_map<std::string, Value> *strings);

  std::shared_ptr<FunctionObject> compile();
  void reset();

  void setTokens(const std::vector<Token> &tokens);
};

#endif // COMPILER_H
