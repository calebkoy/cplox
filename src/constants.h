#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>
#include <stdint.h>

namespace constants {
  constexpr int maxLocals{ std::numeric_limits<uint8_t>::max() + 1 };
  constexpr int callFramesMax{ 64 };
}

#endif
