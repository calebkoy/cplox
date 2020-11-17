#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include "scanner.h"

#include <string>

class ErrorReporter {
  bool inPanicMode{ false };
  bool hadAnError{ false };

public:
  void error(const Token &name, const std::string &message);
  void reset();
  void exitPanicMode();

  bool panicMode();
  bool hadError();
};

#endif // ERROR_REPORTER_H
