#include "error_reporter.h"

void ErrorReporter::report(int line, const std::string& message) {
  std::cerr << "Error at line " << line << ": " << message << '\n';
}
