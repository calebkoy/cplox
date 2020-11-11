#include "error_reporter.h"

#include <iostream>

void ErrorReporter::error(const Token &token, const std::string &message) {
  if (inPanicMode) return;
  inPanicMode = true;

  std::cerr << "[line " << token.line << "] Error";

  if (token.type == TOKEN_EOF) {
    std::cerr << " at end";
  } else if (token.type == TOKEN_ERROR) {
    // Do nothing.
  } else {
    std::cerr << " at " << "'" << token.lexeme << "'";
  }

  std::cerr << ": " << message << '\n';
  hadAnError = true;
}

void ErrorReporter::reset() {
  hadAnError = false;
  inPanicMode = false;
}

void ErrorReporter::exitPanicMode() {
  inPanicMode = false;
}

bool ErrorReporter::panicMode() {
  return inPanicMode;
}

bool ErrorReporter::hadError() {
  return hadAnError;
}
