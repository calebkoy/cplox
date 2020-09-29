#include <iostream>
#include <memory>

#include "compiler.h"

#define DEBUG_PRINT_CODE

// Q: should I be using smart pointers and/or move semantics here
// to ensure that there are no memory leaks?
// See https://stackoverflow.com/questions/7575459/c-should-i-initialize-pointer-members-that-are-assigned-to-in-the-constructor
Compiler::Compiler(const std::vector<Token> tokens, Chunk *chunk,
                   Object *&objects, std::unordered_map<std::string, Value> &strings) :
  tokens{ tokens }, chunk{ chunk }, compilingChunk{ chunk }, objects{ objects }, strings{ strings } {
}

bool Compiler::compile() {
  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }
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

void Compiler::declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }

  if (panicMode) synchronise();
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

uint8_t Compiler::parseVariable(const std::string &errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  return identifierConstant(); // Q: is passing a pointer to the token the best?
}

uint8_t Compiler::identifierConstant() {
  return makeConstant(Value(copyString()));
}

void Compiler::statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
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
    case TOKEN_BANG: unary(); break;
    case TOKEN_BANG_EQUAL: break;
    case TOKEN_EQUAL: break;
    case TOKEN_EQUAL_EQUAL: break;
    case TOKEN_GREATER: break;
    case TOKEN_GREATER_EQUAL: break;
    case TOKEN_LESS: break;
    case TOKEN_LESS_EQUAL: break;
    case TOKEN_IDENTIFIER: variable(); break;
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
void Compiler::invokeInfixRule() {
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

    case TOKEN_LEFT_PAREN:
    case TOKEN_RIGHT_PAREN:
    case TOKEN_LEFT_BRACE:
    case TOKEN_RIGHT_BRACE:
    case TOKEN_COMMA:
    case TOKEN_DOT:
    case TOKEN_SEMICOLON:
    case TOKEN_BANG:
    case TOKEN_EQUAL:
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

void Compiler::string() {
  emitConstant(Value(copyString()));
}

void Compiler::variable() {
  namedVariable();
}

void Compiler::namedVariable() {
  uint8_t arg = identifierConstant();
  emitBytes(OP_GET_GLOBAL, arg);
}

StringObject* Compiler::copyString() {
  // Q: how can I make sure that stringObject gets deleted and memory gets freed at the right time?
  //StringObject* stringObject = new StringObject(previous.lexeme);

  std::unordered_map<std::string, Value>::iterator it = strings.find(previous.lexeme);
  if (it == strings.end()) {
    StringObject* stringObject = new StringObject(previous.lexeme);
    (*stringObject).setNext(objects);
    objects = stringObject;
    //strings.insert(std::make_pair(stringObject, Value{ VAL_NULL }));
    strings.insert(std::make_pair(previous.lexeme, Value{ stringObject }));

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
  int constant = (*compilingChunk).addConstant(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0; // Q: could this be problematic?
  }

  return (uint8_t)constant;
}

void Compiler::defineVariable(uint8_t global) {
  emitBytes(OP_DEFINE_GLOBAL, global);
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
