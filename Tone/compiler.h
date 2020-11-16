#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "environment.h"
#include "error_reporter.h"
#include "scanner.h"

#include <memory>
#include <unordered_map>
#include <vector>

// TODO: make this an enum class
// TODO: consider moving this into the Compiler class declaration
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

// TODO: consider moving this into the Compiler class declaration
struct ClassEnvironment {
  struct ClassEnvironment* enclosing;
  Token name;
  bool hasSuperclass;
};

// TODO: consider adding copy ctor and copy assignment op
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
    {TOKEN_LEFT_PAREN, PRECEDENCE_CALL},
    {TOKEN_RIGHT_PAREN, PRECEDENCE_NONE},
    {TOKEN_LEFT_BRACE, PRECEDENCE_NONE},
    {TOKEN_RIGHT_BRACE, PRECEDENCE_NONE},
    {TOKEN_COMMA, PRECEDENCE_NONE},
    {TOKEN_DOT, PRECEDENCE_CALL},
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
    {TOKEN_AND, PRECEDENCE_AND},
    {TOKEN_CLASS, PRECEDENCE_NONE},
    {TOKEN_ELSE, PRECEDENCE_NONE},
    {TOKEN_FALSE, PRECEDENCE_NONE},
    {TOKEN_FOR, PRECEDENCE_NONE},
    {TOKEN_FUNCTION, PRECEDENCE_NONE},
    {TOKEN_IF, PRECEDENCE_NONE},
    {TOKEN_NULL, PRECEDENCE_NONE},
    {TOKEN_OR, PRECEDENCE_OR},
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
  void namedVariable(Token name, bool canAssign); // TODO: pass Token by const ref
  void varDeclaration();
  void classDeclaration();
  uint8_t parseVariable(const std::string &errorMessage);
  uint8_t identifierConstant(Token* name); // TODO: pass Token by const ref?
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
  // TODO: pass Tokens by const ref?
  bool identifiersEqual(Token* a, Token* b); // TODO: consider extracting this; it's also used in Environment
  void string();
  //StringObject* copyString(Token* name); // TODO: remove if everything works using version that returns smart ptr
  std::shared_ptr<StringObject> copyString(Token* name); // TODO: pass Token by const ref?
  void invokePrefixRule(bool canAssign);
  void invokeInfixRule(bool canAssign);
  void parsePrecedence(Precedence precedence);
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  void emitConstant(Value value); // TODO: pass Value by const ref? It has a shared_ptr
  uint8_t makeConstant(Value value); // TODO: pass Value by const ref? It has a shared_ptr
  void emitReturn();
  FunctionObject* endCompiler();

public:
  Compiler(std::unordered_map<std::string, Value> *strings);

  FunctionObject* compile();
  void reset();

  void setTokens(const std::vector<Token> &tokens); // TODO: make position of '&' and '*' consistent in the project
};

#endif // COMPILER_H
