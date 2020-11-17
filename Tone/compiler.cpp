#include "chunk.h"
#include "compiler.h"
#include "value.h"

#include <iostream>
#include <limits>
#include <memory>
#include <utility>

//#define DEBUG_PRINT_CODE

Compiler::Compiler(std::unordered_map<std::string, Value> *strings) :
  strings{ strings },
  currentEnvironment{ std::make_unique<Environment>(TYPE_SCRIPT, nullptr, reporter) }
{
}

std::shared_ptr<FunctionObject> Compiler::compile() {
  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }

  auto function = endCompiler();
  return reporter.hadError() ? nullptr : function;
}

void Compiler::advance() {
  previous = current;
  for (;;) {
    current = tokens.at(currentTokenIndex++);
    if (current.type != TOKEN_ERROR) break;

    reporter.error(current, current.lexeme);
  }
}

void Compiler::declaration() {
  if (match(TOKEN_CLASS)) {
    classDeclaration();
  } else if (match(TOKEN_FUNCTION)) {
    functionDeclaration();
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }

  if (reporter.panicMode()) synchronise();
}

void Compiler::functionDeclaration() {
  uint8_t global = parseVariable("Expect function name.");
  markInitialised();
  function(TYPE_FUNCTION);
  defineVariable(global);
}

void Compiler::function(FunctionType type) {
  currentEnvironment = std::make_unique<Environment>(type,
                                                     std::move(currentEnvironment),
                                                     reporter);

  if (type != TYPE_SCRIPT) {
    currentEnvironment->getFunction()->setName(copyString(previous));
  }

  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      currentEnvironment->getFunction()->incrementArity();
      if (currentEnvironment->getFunction()->getArity() > 255) {
        reporter.error(current, "Cannot have more than 255 parameters.");
      }

      uint8_t paramConstant = parseVariable("Expect parameter name.");
      defineVariable(paramConstant);
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
  block();
  auto upvalues = currentEnvironment->releaseUpvalues();
  auto function = endCompiler();
  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_CLOSURE),
            makeConstant(Value{ function }));

  for (const auto &upvalue : upvalues) {
    emitByte(upvalue.isLocal ? 1 : 0);
    emitByte(upvalue.index);
  }
}

void Compiler::varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");
  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_NULL));
  }

  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
  defineVariable(global);
}

void Compiler::classDeclaration() {
  consume(TOKEN_IDENTIFIER, "Expect class name.");
  Token className = previous;
  uint8_t nameConstant = identifierConstant(previous);
  declareVariable();
  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_CLASS),
            nameConstant);

  defineVariable(nameConstant);

  ClassEnvironment classEnvironment;
  classEnvironment.name = previous;
  classEnvironment.hasSuperclass = false;
  classEnvironment.enclosing = currentClassEnvironment;
  currentClassEnvironment = &classEnvironment;
  if (match(TOKEN_LESS)) {
    consume(TOKEN_IDENTIFIER, "Expect superclass name.");
    variable(false);
    if (identifiersEqual(className, previous)) {
      reporter.error(previous, "A class cannot inherit from itself.");
    }

    beginScope();
    if (currentEnvironment->localCountAtMax()) {
      reporter.error(previous, "Too many local variables in function.");
    } else {
      currentEnvironment->addLocal(syntheticToken("super"));
    }

    defineVariable(0);
    namedVariable(className, false);
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_INHERIT));
    classEnvironment.hasSuperclass = true;
  }

  // Load the class onto the stack before compiling
  // its body, so that its methods can be bound to it.
  namedVariable(className, false);
  consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    method();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  if (classEnvironment.hasSuperclass) {
    endScope();
  }

  currentClassEnvironment = currentClassEnvironment->enclosing;
}

Token Compiler::syntheticToken(const std::string &text) {
  Token token;
  token.lexeme = text;
  token.length = text.length();
  return token;
}

void Compiler::method() {
  consume(TOKEN_IDENTIFIER, "Expect method name.");
  uint8_t constant = identifierConstant(previous);

  FunctionType type = TYPE_METHOD;
  if (previous.length == 4 &&
      previous.lexeme.compare("init") == 0) {
    type = TYPE_INITIALIZER;
  }

  function(type);
  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_METHOD),
            constant);
}

uint8_t Compiler::parseVariable(const std::string &errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  declareVariable();
  if (currentEnvironment->getScopeDepth() > 0) return 0;

  return identifierConstant(previous);
}

uint8_t Compiler::identifierConstant(const Token &name) {
  return makeConstant(Value(copyString(name)));
}

void Compiler::statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_RETURN)) {
    returnStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

void Compiler::ifStatement() {
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  int thenJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP_IF_FALSE));
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  statement();
  int elseJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP));
  patchJump(thenJump);
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  if (match(TOKEN_ELSE)) statement();
  patchJump(elseJump);
}

void Compiler::whileStatement() {
  int loopStart = currentChunk()->getBytecodeCount();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  int exitJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP_IF_FALSE));
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  statement();
  emitLoop(loopStart);
  patchJump(exitJump);
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
}

void Compiler::forStatement() {
  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  if (match(TOKEN_SEMICOLON)) {
    // No initializer.
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    expressionStatement();
  }

  int loopStart = currentChunk()->getBytecodeCount();
  int exitJump = -1;
  if (!match(TOKEN_SEMICOLON)) {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    // Jump out of the loop if the condition is false.
    exitJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP_IF_FALSE));
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  }

  if (!match(TOKEN_RIGHT_PAREN)) {
    int bodyJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP));
    int incrementStart = currentChunk()->getBytecodeCount();
    expression();
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");
    emitLoop(loopStart);
    loopStart = incrementStart;
    patchJump(bodyJump);
  }

  statement();
  emitLoop(loopStart);
  if (exitJump != -1) {
    patchJump(exitJump);
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  }

  endScope();
}

void Compiler::returnStatement() {
  if (currentEnvironment->getFunctionType() == TYPE_SCRIPT) {
    reporter.error(previous, "Cannot return from top-level code.");
  }

  if (match(TOKEN_SEMICOLON)) {
    emitReturn();
  } else {
    if (currentEnvironment->getFunctionType() == TYPE_INITIALIZER) {
      reporter.error(previous, "Cannot return a value from an initializer.");
    }

    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_RETURN));
  }
}

void Compiler::emitLoop(int loopStart) {
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_LOOP));
  int offset = currentChunk()->getBytecodeCount() - loopStart + 2;
  if (offset > std::numeric_limits<uint16_t>::max()) {
    reporter.error(previous, "Loop body too large.");
  }

  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}

int Compiler::emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk()->getBytecodeCount() - 2;
}

void Compiler::patchJump(int offset) {
  int jump = currentChunk()->getBytecodeCount() - offset - 2;
  if (jump > std::numeric_limits<uint16_t>::max()) {
    reporter.error(previous, "Too much code to jump over.");
  }

  currentChunk()->setBytecodeValue(offset, (jump >> 8) & 0xff);
  currentChunk()->setBytecodeValue(offset+1, jump & 0xff);
}

void Compiler::block() {
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::beginScope() {
  currentEnvironment->incrementScopeDepth();
}

void Compiler::endScope() {
  currentEnvironment->decrementScopeDepth();

  Local *local{ nullptr };
  while (currentEnvironment->getLocalCount() > 0 &&
         (local = currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1))->depth >
            currentEnvironment->getScopeDepth()) {

    if (local->isCaptured) {
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_CLOSE_UPVALUE));
    } else {
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
    }
    currentEnvironment->decrementLocalCount();
  }
}

void Compiler::synchronise() {
  reporter.exitPanicMode();
  while (current.type != TOKEN_EOF) {
    if (previous.type == TOKEN_SEMICOLON) return;

    switch (current.type) {
      case TOKEN_CLASS:
      case TOKEN_FUNCTION:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;

      default:
        break;
    }

    advance();
  }
}

void Compiler::printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_PRINT));
}

void Compiler::expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
}

bool Compiler::match(TokenType type) {
  if (type == TOKEN_EOF && check(type)) return true;
  if (!check(type)) return false;

  advance();
  return true;
}

bool Compiler::check(TokenType type) {
  return current.type == type;
}

void Compiler::expression() {
  parsePrecedence(Precedence::ASSIGNMENT);
}

void Compiler::parsePrecedence(Precedence precedence) {
  advance();
  bool canAssign = precedence <= Precedence::ASSIGNMENT;
  invokePrefixRule(canAssign);
  while (precedence <= tokenPrecedence.at(current.type)) {
    advance();
    invokeInfixRule(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    reporter.error(previous, "Invalid assignment target.");
  }
}

void Compiler::invokePrefixRule(bool canAssign) {
  switch (previous.type) {
    case TOKEN_MINUS:
    case TOKEN_BANG:
      unary(); break;

    case TOKEN_TRUE:
    case TOKEN_FALSE:
    case TOKEN_NULL:
      literal(); break;

    case TOKEN_SUPER: super_(); break;
    case TOKEN_THIS: this_(); break;
    case TOKEN_LEFT_PAREN: grouping(); break;
    case TOKEN_IDENTIFIER: variable(canAssign); break;
    case TOKEN_STRING: string(); break;
    case TOKEN_NUMBER: number(); break;

    case TOKEN_RIGHT_PAREN:
    case TOKEN_LEFT_BRACE:
    case TOKEN_RIGHT_BRACE:
    case TOKEN_COMMA:
    case TOKEN_DOT:
    case TOKEN_PLUS:
    case TOKEN_SEMICOLON:
    case TOKEN_SLASH:
    case TOKEN_STAR:
    case TOKEN_BANG_EQUAL:
    case TOKEN_EQUAL:
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
    case TOKEN_AND:
    case TOKEN_CLASS:
    case TOKEN_ELSE:
    case TOKEN_FOR:
    case TOKEN_FUNCTION:
    case TOKEN_IF:
    case TOKEN_OR:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
    case TOKEN_VAR:
    case TOKEN_WHILE:
    case TOKEN_ERROR:
    case TOKEN_EOF:
      reporter.error(previous, "Expect expression.");
      break;

    default:
      reporter.error(previous,
                     "Unexpected token type. No corresponding prefix rule.");
      break;
  }
}

void Compiler::invokeInfixRule(bool canAssign) {
  switch (previous.type) {
    case TOKEN_MINUS:
    case TOKEN_PLUS:
    case TOKEN_SLASH:
    case TOKEN_STAR:
    case TOKEN_BANG_EQUAL:
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
      binary();
      break;

    case TOKEN_AND: and_(); break;
    case TOKEN_OR: or_(); break;

    case TOKEN_DOT: dot(canAssign); break;

    case TOKEN_LEFT_PAREN: call(); break;

    case TOKEN_RIGHT_PAREN:
    case TOKEN_LEFT_BRACE:
    case TOKEN_RIGHT_BRACE:
    case TOKEN_COMMA:
    case TOKEN_SEMICOLON:
    case TOKEN_BANG:
    case TOKEN_EQUAL:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
    case TOKEN_NUMBER:
    case TOKEN_CLASS:
    case TOKEN_ELSE:
    case TOKEN_FALSE:
    case TOKEN_FOR:
    case TOKEN_FUNCTION:
    case TOKEN_IF:
    case TOKEN_NULL:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
    case TOKEN_SUPER:
    case TOKEN_THIS:
    case TOKEN_TRUE:
    case TOKEN_VAR:
    case TOKEN_WHILE:
    case TOKEN_ERROR:
    case TOKEN_EOF:
      break;

    default:
      reporter.error(previous,
                     "Unexpected token type. No corresponding infix rule.");
      break;
  }
}

void Compiler::number() {
  emitConstant(Value{ Value::ValueType::VAL_NUMBER,
                      std::stod(previous.lexeme) });
}

void Compiler::and_() {
  int endJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP_IF_FALSE));
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  parsePrecedence(Precedence::AND);
  patchJump(endJump);
}

void Compiler::or_() {
  int elseJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP_IF_FALSE));
  int endJump = emitJump(static_cast<unsigned int>(Chunk::OpCode::OP_JUMP));
  patchJump(elseJump);
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_POP));
  parsePrecedence(Precedence::OR);
  patchJump(endJump);
}

void Compiler::this_() {
  if (currentClassEnvironment == nullptr) {
    reporter.error(previous, "Cannot use 'this' outside of a class.");
    return;
  }

  variable(false);
}

void Compiler::super_() {
  if (currentClassEnvironment == nullptr) {
    reporter.error(previous, "Cannot use 'super' outside of a class.");
  } else if (!(currentClassEnvironment->hasSuperclass)) {
    reporter.error(previous, "Cannot use 'super' in a class with no superclass.");
  }

  consume(TOKEN_DOT, "Expect '.' after 'super'.");
  consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
  uint8_t name = identifierConstant(previous);
  namedVariable(syntheticToken("this"), false);
  if (match(TOKEN_LEFT_PAREN)) {
    uint8_t argCount = argumentList();
    namedVariable(syntheticToken("super"), false);
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_SUPER_INVOKE),
              name);

    emitByte(argCount);
  } else {
    namedVariable(syntheticToken("super"), false);
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_GET_SUPER),
              name);
  }
}

void Compiler::call() {
  uint8_t argCount = argumentList();
  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_CALL),
            argCount);
}

void Compiler::dot(bool canAssign) {
  consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
  uint8_t name = identifierConstant(previous);
  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_SET_PROPERTY),
              name);
  } else if (match(TOKEN_LEFT_PAREN)) {
    uint8_t argCount = argumentList();
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_INVOKE),
              name);

    emitByte(argCount);
  } else {
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_GET_PROPERTY),
              name);
  }
}

uint8_t Compiler::argumentList() {
  uint8_t argCount = 0;
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expression();
      if (argCount == 255) {
        reporter.error(previous, "Cannot have more than 255 arguments.");
      }

      argCount++;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return argCount;
}

void Compiler::literal() {
  switch (previous.type) {
    case TOKEN_FALSE:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_FALSE));
      break;
    case TOKEN_NULL:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_NULL));
      break;
    case TOKEN_TRUE:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_TRUE));
      break;
    default:
      return;
  }
}

void Compiler::string() {
  emitConstant(Value(copyString(previous)));
}

void Compiler::variable(bool canAssign) {
  namedVariable(previous, canAssign);
}

void Compiler::namedVariable(const Token &name, bool canAssign) {
  uint8_t getOp, setOp;
  int arg = currentEnvironment->resolveLocal(name);
  if (arg != -1) {
    getOp = static_cast<unsigned int>(Chunk::OpCode::OP_GET_LOCAL);
    setOp = static_cast<unsigned int>(Chunk::OpCode::OP_SET_LOCAL);
  } else if ((arg = currentEnvironment->resolveUpvalue(name)) != -1) {
    getOp = static_cast<unsigned int>(Chunk::OpCode::OP_GET_UPVALUE);
    setOp = static_cast<unsigned int>(Chunk::OpCode::OP_SET_UPVALUE);
  } else {
    arg = identifierConstant(name);
    getOp = static_cast<unsigned int>(Chunk::OpCode::OP_GET_GLOBAL);
    setOp = static_cast<unsigned int>(Chunk::OpCode::OP_SET_GLOBAL);
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(setOp, (uint8_t)arg);
  } else {
    emitBytes(getOp, (uint8_t)arg);
  }
}

std::shared_ptr<StringObject> Compiler::copyString(const Token &name) {
  std::unordered_map<std::string, Value>::iterator it = strings->find(name.lexeme);
  if (it == strings->end()) {
    auto stringObject{ std::make_shared<StringObject>(name.lexeme) };
    Value value{ stringObject };
    strings->insert(std::make_pair(name.lexeme, value));
    return stringObject;
  }

  return (it->second).asString();
}

void Compiler::emitConstant(const Value &value) {
  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_CONSTANT),
            makeConstant(value));
}

uint8_t Compiler::makeConstant(const Value &value) {
  auto constant = static_cast<unsigned int>(currentChunk()->addConstant(value));
  if (constant > std::numeric_limits<uint8_t>::max()) {
    reporter.error(previous, "Too many constants in one chunk.");
    return 0;
  }

  return constant;
}

void Compiler::declareVariable() {
  if (currentEnvironment->getScopeDepth() == 0) return;

  if (currentEnvironment->localCountAtMax()) {
    reporter.error(previous, "Too many local variables in function.");
    return;
  }

  for (int i = currentEnvironment->getLocalCount() - 1; i >= 0; i--) {
    Local* local = currentEnvironment->getLocal(i);
    if (local->depth != -1 && local->depth < currentEnvironment->getScopeDepth()) {
      break;
    }

    if (identifiersEqual(previous, local->name)) {
      reporter.error(previous, "Already variable with this name in this scope.");
    }
  }

  currentEnvironment->addLocal(previous);
}

bool Compiler::identifiersEqual(const Token &a, const Token &b) {
  if (a.length != b.length) return false;
  return a.lexeme.compare(b.lexeme) == 0;
}

void Compiler::defineVariable(uint8_t global) {
  if (currentEnvironment->getScopeDepth() > 0) {
    markInitialised();
    return;
  }

  emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_DEFINE_GLOBAL),
            global);
}

void Compiler::markInitialised() {
  if (currentEnvironment->getScopeDepth() == 0) return;

  currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1)->depth = currentEnvironment->getScopeDepth();
}

void Compiler::consume(TokenType type, const std::string &message) {
  if (type == TOKEN_EOF && current.type == type) return;

  if (current.type == type) {
    advance();
    return;
  }

  reporter.error(current, message);
}

void Compiler::grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary() {
  TokenType operatorType = previous.type;
  parsePrecedence(Precedence::UNARY);
  switch (operatorType) {
    case TOKEN_BANG:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_NOT));
      break;
    case TOKEN_MINUS:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_NEGATE));
      break;
    default:
      return;
  }
}

void Compiler::binary() {
  TokenType operatorType = previous.type;
  int precedence = static_cast<int>(tokenPrecedence.at(operatorType));
  parsePrecedence(static_cast<Precedence>(precedence + 1));
  switch (operatorType) {
    case TOKEN_BANG_EQUAL:
      emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_EQUAL),
                static_cast<unsigned int>(Chunk::OpCode::OP_NOT));
      break;
    case TOKEN_EQUAL_EQUAL:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_EQUAL));
      break;
    case TOKEN_GREATER:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_GREATER));
      break;
    case TOKEN_GREATER_EQUAL:
      emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_LESS),
                static_cast<unsigned int>(Chunk::OpCode::OP_NOT));
      break;
    case TOKEN_LESS:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_LESS));
      break;
    case TOKEN_LESS_EQUAL:
      emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_GREATER),
                static_cast<unsigned int>(Chunk::OpCode::OP_NOT));
      break;
    case TOKEN_PLUS:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_ADD));
      break;
    case TOKEN_MINUS:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_SUBTRACT));
      break;
    case TOKEN_STAR:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_MULTIPLY));
      break;
    case TOKEN_SLASH:
      emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_DIVIDE));
      break;
    default:
      return;
  }
}

void Compiler::emitByte(uint8_t byte) {
  currentChunk()->appendByte(byte, previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

std::shared_ptr<FunctionObject> Compiler::endCompiler() {
  emitReturn();
  auto function = currentEnvironment->getFunction();

#ifdef DEBUG_PRINT_CODE
  if (!reporter.hadError()) {
    currentChunk()->disassemble(function->getName() != NULL ?
                                function->getName()->getChars() :
                                "<script>");
  }
#endif // DEBUG_PRINT_CODE

  currentEnvironment = currentEnvironment->releaseEnclosing();
  return function;
}

void Compiler::emitReturn() {
  if (currentEnvironment->getFunctionType() == TYPE_INITIALIZER) {
    emitBytes(static_cast<unsigned int>(Chunk::OpCode::OP_GET_LOCAL), 0);
  } else {
    emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_NULL));
  }
  emitByte(static_cast<unsigned int>(Chunk::OpCode::OP_RETURN));
}

Chunk* Compiler::currentChunk() {
  return currentEnvironment->getFunction()->getChunk();
}

void Compiler::reset() {
  currentTokenIndex = 0;
  reporter.reset();
}

void Compiler::setTokens(const std::vector<Token> &tokens) {
  this->tokens = tokens;
}
