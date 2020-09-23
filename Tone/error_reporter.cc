#include "error_reporter.h"

void ErrorReporter::report(const std::string& message) {
  std::cerr << message << '\n';
}
