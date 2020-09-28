#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include <iostream>
#include <string>

class ErrorReporter {
public:
  void report(int line, const std::string& message);
};

#endif // ERROR_REPORTER_H