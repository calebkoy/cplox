#include <iostream>
#include <limits>
#include <memory>

#include "compiler.h"

#define DEBUG_PRINT_CODE

// #define DEBUG_ENV_CODE

// Q: should there be a default constructor that sets pointer members to nullptr?

// Q: should I be using smart pointers and/or move semantics here
// to ensure that there are no memory leaks?
// See https://stackoverflow.com/questions/7575459/c-should-i-initialize-pointer-members-that-are-assigned-to-in-the-constructor
Compiler::Compiler(const std::vector<Token> tokens,
                   Object *&objects, std::unordered_map<std::string, Value> *strings) :
  tokens{ tokens }, objects{ objects }, strings{ strings } {

  currentEnvironment = new Environment(TYPE_SCRIPT, nullptr); // Q: how can I prevent memory leaks?
}

FunctionObject* Compiler::compile() {
  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }

  FunctionObject* function = endCompiler();
  return hadError ? NULL : function; // Q: could it be nullptr instead of NULL?
}

void Compiler::advance() {
  previous = current;

  for (;;) {
    current = tokens.at(currentTokenIndex++);
    if (current.type != TOKEN_ERROR) break;

    errorAtCurrent(current.lexeme);
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

  if (panicMode) synchronise();
}

void Compiler::functionDeclaration() {
  uint8_t global = parseVariable("Expect function name.");
  markInitialised();
  function(TYPE_FUNCTION);
  defineVariable(global);
}

void Compiler::function(FunctionType type) {
#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just entered function()\n" ;
  std::cout << "Current env's function name: ";
  if (currentEnvironment == NULL) {
    std::cout << "null" << '\n';
  } else if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
    std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
    std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
    std::cout << "Current env locals: " << '\n';
    for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
      std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
    }
    std::cout << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
    std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
    std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
    std::cout << "Current env locals: " << '\n';
    for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
      std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
    }
    std::cout << '\n';
  }
#endif // DEBUG_ENV_CODE

  Environment* environment = new Environment{ type, currentEnvironment }; // Q: how to ensure memory isn't leaked?
  currentEnvironment = environment;

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just re-assigned current env\n";
  std::cout << "Current env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';

  // Debug the current env
  std::cout << "\nEnclosing env's function name: ";
  if (currentEnvironment->getEnclosing() == NULL) {
    std::cout << "enclosing env is null\n";
  } else if (currentEnvironment->getEnclosing()->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getEnclosing()->getFunction()->getName()->getChars() << '\n';
  }
#endif

  if (type != TYPE_SCRIPT) {
    currentEnvironment->getFunction()->setName(copyString(&previous));
  }

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Current env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';

  // Debug the current env
  std::cout << "\nEnclosing env's function name: ";
  if (currentEnvironment->getEnclosing() == NULL) {
    std::cout << "enclosing env is null\n";
  } else if (currentEnvironment->getEnclosing()->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getEnclosing()->getFunction()->getName()->getChars() << '\n';
  }
#endif // DEBUG_ENV_CODE

  beginScope();

  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      currentEnvironment->getFunction()->incrementArity();
      if (currentEnvironment->getFunction()->getArity() > 255) {
        errorAtCurrent("Cannot have more than 255 parameters.");
      }

      uint8_t paramConstant = parseVariable("Expect parameter name.");
      defineVariable(paramConstant);
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");

  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
  block();

  FunctionObject* function = endCompiler();
  emitBytes(OP_CLOSURE, makeConstant(Value{ function }));

  for (int i = 0; i < function->getUpvalueCount(); i++) {
    emitByte(environment->getUpvalue(i)->isLocal ? 1 : 0);
    emitByte(environment->getUpvalue(i)->index);
  }
}

void Compiler::varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NULL);
  }
  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  defineVariable(global);
}

void Compiler::classDeclaration() {
  consume(TOKEN_IDENTIFIER, "Expect class name.");
  uint8_t nameConstant = identifierConstant(&previous);
  declareVariable();

  emitBytes(OP_CLASS, nameConstant);
  defineVariable(nameConstant);

  consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
  consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
}

uint8_t Compiler::parseVariable(const std::string &errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);

  declareVariable();
  if (currentEnvironment->getScopeDepth() > 0) return 0;

  return identifierConstant(&previous); // Q: is passing a pointer to the token the best?
}

uint8_t Compiler::identifierConstant(Token* name) {
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

  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);

  if (match(TOKEN_ELSE)) statement();
  patchJump(elseJump);
}

void Compiler::whileStatement() {
  int loopStart = currentChunk()->getBytecodeCount();

  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

  int exitJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  statement();

  emitLoop(loopStart);

  patchJump(exitJump);
  emitByte(OP_POP);
}

void Compiler::forStatement() {
  beginScope();

  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  if (match(TOKEN_SEMICOLON)) {
    // No initializer. // Q: is this necessary?
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
    exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP); // Condition.
  }

  if (!match(TOKEN_RIGHT_PAREN)) {
    int bodyJump = emitJump(OP_JUMP);

    int incrementStart = currentChunk()->getBytecodeCount();
    expression();
    emitByte(OP_POP);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    emitLoop(loopStart);
    loopStart = incrementStart;
    patchJump(bodyJump);
  }

  statement();

  emitLoop(loopStart);

  if (exitJump != -1) {
    patchJump(exitJump);
    emitByte(OP_POP); // Condition.
  }

  endScope();
}

void Compiler::returnStatement() {
  if (currentEnvironment->getFunctionType() == TYPE_SCRIPT) {
    error("Cannot return from top-level code.");
  }

  if (match(TOKEN_SEMICOLON)) {
    emitReturn();
  } else {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    emitByte(OP_RETURN);
  }
}

void Compiler::emitLoop(int loopStart) {
  emitByte(OP_LOOP);

  int offset = currentChunk()->getBytecodeCount() - loopStart + 2;
  if (offset > std::numeric_limits<uint16_t>::max()) error("Loop body too large.");

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
    error("Too much code to jump over.");
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

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just incremented scope depth" << '\n';
  std::cout << "Current env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';
#endif // DEBUG_ENV_CODE
}

void Compiler::endScope() {
  currentEnvironment->decrementScopeDepth();

  while (currentEnvironment->getLocalCount() > 0 &&
         (currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1))->depth >
            currentEnvironment->getScopeDepth()) {
    if (currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1)->isCaptured) {
      emitByte(OP_CLOSE_UPVALUE);
    } else {
      emitByte(OP_POP);
    }
    currentEnvironment->decrementLocalCount();
  }
}

void Compiler::synchronise() {
  panicMode = false;

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
        // Q: should there be a break here, for cleanliness?
        // Do nothing.
        ;
    }

    advance();
  }
}

void Compiler::printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}

void Compiler::expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
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
  parsePrecedence(PRECEDENCE_ASSIGNMENT);
}

void Compiler::parsePrecedence(Precedence precedence) {
  advance();
  bool canAssign = precedence <= PRECEDENCE_ASSIGNMENT;
  invokePrefixRule(canAssign);
  while (precedence <= tokenPrecedence[current.type]) {
    advance();
    invokeInfixRule(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target.");
  }
}

void Compiler::invokePrefixRule(bool canAssign) {
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
    case TOKEN_BANG: unary(); break;
    case TOKEN_BANG_EQUAL: break;
    case TOKEN_EQUAL: break;
    case TOKEN_EQUAL_EQUAL: break;
    case TOKEN_GREATER: break;
    case TOKEN_GREATER_EQUAL: break;
    case TOKEN_LESS: break;
    case TOKEN_LESS_EQUAL: break;
    case TOKEN_IDENTIFIER: variable(canAssign); break;
    case TOKEN_STRING: string(); break;
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

void Compiler::and_() {
  int endJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  parsePrecedence(PRECEDENCE_AND);

  patchJump(endJump);
}

void Compiler::or_() {
  int elseJump = emitJump(OP_JUMP_IF_FALSE);
  int endJump = emitJump(OP_JUMP);

  patchJump(elseJump);
  emitByte(OP_POP);

  parsePrecedence(PRECEDENCE_OR);
  patchJump(endJump);
}

void Compiler::call() {
  uint8_t argCount = argumentList();
  emitBytes(OP_CALL, argCount);
}

void Compiler::dot(bool canAssign) {
  consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
  uint8_t name = identifierConstant(&previous);

  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(OP_SET_PROPERTY, name);
  } else {
    emitBytes(OP_GET_PROPERTY, name);
  }
}

uint8_t Compiler::argumentList() {
  uint8_t argCount = 0;
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expression();

      if (argCount == 255) {
        error("Cannot have more than 255 arguments.");
      }
      argCount++;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return argCount;
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

void Compiler::string() {
  emitConstant(Value(copyString(&previous)));
}

void Compiler::variable(bool canAssign) {
  namedVariable(previous, canAssign);
}

void Compiler::namedVariable(Token name, bool canAssign) {
  uint8_t getOp, setOp;
  int arg = resolveLocal(currentEnvironment, &name); // does environment need to be passed?
  if (arg != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  } else if ((arg = resolveUpvalue(currentEnvironment, &name)) != -1) {
    getOp = OP_GET_UPVALUE;
    setOp = OP_SET_UPVALUE;
  } else {
    arg = identifierConstant(&name); // Q: why pass a pointer?
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(setOp, (uint8_t)arg);
  } else {
    emitBytes(getOp, (uint8_t)arg);
  }
}

int Compiler::resolveLocal(Environment* environment, Token* name) {
  for (int i = environment->getLocalCount() - 1; i >= 0; i--) {
    Local* local = environment->getLocal(i);
    if (identifiersEqual(name, &local->name)) {
      if (local->depth == -1) {
        error("Cannot read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}

int Compiler::resolveUpvalue(Environment* environment, Token* name) {
  if (environment->getEnclosing() == NULL) return -1;

  int local = resolveLocal(environment->getEnclosing(), name);
  if (local != -1) {
    environment->getEnclosing()->getLocal(local)->isCaptured = true;
    return addUpvalue(environment, (uint8_t)local, true);
  }

  int upvalue = resolveUpvalue(environment->getEnclosing(), name);
  if (upvalue != -1) {
    return addUpvalue(environment, (uint8_t)upvalue, false);
  }

  return -1;
}

int Compiler::addUpvalue(Environment* environment, uint8_t index, bool isLocal) {
  int upvalueCount = environment->getFunction()->getUpvalueCount();

  for (int i = 0; i < upvalueCount; i++) {
    Upvalue* upvalue = environment->getUpvalue(i);
    if (upvalue->index == index && upvalue->isLocal == isLocal) {
      return i;
    }
  }

  if (upvalueCount == Environment::getUint8Count()) {
    error("Too many closure variables in function.");
    return 0;
  }

  environment->getUpvalue(upvalueCount)->isLocal = isLocal; // Q: should getUpvalue return a pointer instead?
  environment->getUpvalue(upvalueCount)->index = index;
  environment->getFunction()->incrementUpvalueCount();

  return upvalueCount;
}

// Q: consider moving copystring to another class since VM uses a similar function
StringObject* Compiler::copyString(Token* name) {
  // Q: how can I make sure that stringObject gets deleted and memory gets freed at the right time?

  std::unordered_map<std::string, Value>::iterator it = strings->find(name->lexeme);
  if (it == strings->end()) {
    StringObject* stringObject = new StringObject(name->lexeme);
    (*stringObject).setNext(objects);
    objects = stringObject;
    strings->insert(std::make_pair(name->lexeme, Value{ stringObject }));

    return stringObject;
  }

  return (it->second).asString(); // Q: are parentheses necessary?
}

// Q: is this function necessary?
void* Compiler::reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) exit(1); // Q: can I do something better than exit(1)?
  return result;
}

// Q: should this class be passed by value?
void Compiler::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t Compiler::makeConstant(Value value) {
  int constant = currentChunk()->addConstant(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0; // Q: could this be problematic?
  }

  return (uint8_t)constant;
}

void Compiler::declareVariable() {
  if (currentEnvironment->getScopeDepth() == 0) return;

  if (currentEnvironment->localCountAtMax()) {
    error("Too many local variables in function.");
    return;
  }

  Token* name = &previous; // Q: why create a pointer and then dereference it?

  for (int i = currentEnvironment->getLocalCount() - 1; i >= 0; i--) {
    // Q: parentheses necessary?
    // Q: why a pointer?
    Local* local = currentEnvironment->getLocal(i);

    if (local->depth != -1 && local->depth < currentEnvironment->getScopeDepth()) {
      break;
    }

    if (identifiersEqual(name, &local->name)) {
      error("Variable with this name already declared in this scope.");
    }
  }

  currentEnvironment->addLocal(*name);

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just added local to current env" << '\n';
  std::cout << "Current env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';
#endif // DEBUG_ENV_CODE
}

bool Compiler::identifiersEqual(Token* a, Token* b) {
  if (a->length != b->length) return false;
  return a->lexeme.compare(b->lexeme) == 0;
}

void Compiler::defineVariable(uint8_t global) {
  if (currentEnvironment->getScopeDepth() > 0) {
    markInitialised();
    return;
  }

  emitBytes(OP_DEFINE_GLOBAL, global);
}

void Compiler::markInitialised() {
  if (currentEnvironment->getScopeDepth() == 0) return;

#ifdef DEBUG_ENV_CODE
  std::cout << "Depth of local at index " << currentEnvironment->getLocalCount() - 1 << ": " <<
            currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1)->depth << '\n';
#endif // DEBUG_ENV_CODE

  currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1)->depth = currentEnvironment->getScopeDepth();

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just updated the depth of the local at index " << currentEnvironment->getLocalCount()-1 << '\n';
  std::cout << "New depth: " << currentEnvironment->getLocal(currentEnvironment->getLocalCount() - 1)->depth << '\n';

  std::cout << "\nCurrent env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';
#endif // DEBUG_ENV_CODE
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
    case TOKEN_BANG: emitByte(OP_NOT); break;
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
    case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TOKEN_GREATER:       emitByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS:          emitByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
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

FunctionObject* Compiler::endCompiler() {
  emitReturn();
  FunctionObject* function = currentEnvironment->getFunction();

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just got the current env's function\n";
  std::cout << "Current env's function name: ";
  if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
  }
  std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
  std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
  std::cout << "Current env locals: " << '\n';
  for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
    std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
  }
  std::cout << '\n';
#endif // DEBUG_ENV_CODE

#ifdef DEBUG_PRINT_CODE
  if (!hadError) currentChunk()->disassemble();
#endif // DEBUG_PRINT_CODE

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "\nEnclosing env's function name: ";
  if (currentEnvironment->getEnclosing() == NULL) {
    std::cout << "enclosing env is null\n";
  } else if (currentEnvironment->getEnclosing()->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
  } else {
    std::cout << currentEnvironment->getEnclosing()->getFunction()->getName()->getChars() << '\n';
  }
#endif // DEBUG_ENV_CODE

  currentEnvironment = currentEnvironment->getEnclosing();

#ifdef DEBUG_ENV_CODE
  // Debug the current env
  std::cout << "Just set current env to the enclosing env\n";
  std::cout << "Current env's function name: ";
  if (currentEnvironment == NULL) {
    std::cout << "null" << '\n';
  } else if (currentEnvironment->getFunction()->getName() == NULL) {
    std::cout << "null" << '\n';
    std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
    std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
    std::cout << "Current env locals: " << '\n';
    for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
      std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
    }
    std::cout << '\n';
  } else {
    std::cout << currentEnvironment->getFunction()->getName()->getChars() << '\n';
    std::cout << "Current env scope depth: " << currentEnvironment->getScopeDepth() << '\n';
    std::cout << "Current env local count: " << currentEnvironment->getLocalCount() << '\n';
    std::cout << "Current env locals: " << '\n';
    for (int i = 0; i < currentEnvironment->getLocalCount(); i++) {
      std::cout << currentEnvironment->getLocal(i)->name.lexeme << '\n';
    }
    std::cout << '\n';
  }
#endif // DEBUG_ENV_CODE

  return function;
}

void Compiler::emitReturn() {
  emitByte(OP_NULL);
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

// Q: is it better to return a pointer to a Chunk?
Chunk* Compiler::currentChunk() {
  return currentEnvironment->getFunction()->getChunk();
}
