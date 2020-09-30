#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <array>
#include <limits>

#include "scanner.h"

typedef struct {
  Token name;
  int depth;
} Local;

class Environment {
  static const int uint8Count = std::numeric_limits<uint8_t>::max();
  std::array<Local, uint8Count+1> locals;
  int scopeDepth;
  int localCount;

public:
  Environment();

  void incrementScopeDepth();
  void decrementScopeDepth();
  void decrementLocalCount();
  void addLocal(Token name);
  bool localCountAtMax();
  Local* getLocal(int index);

  int getScopeDepth();
  int getLocalCount();
};

#endif // ENVIRONMENT_H
