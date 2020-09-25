#ifndef CHUNK_H
#define CHUNK_H

#include <map>
#include <string>
#include <vector>

#include "value.h"

enum OpCode {
  OP_CONSTANT,
  OP_RETURN,
};

struct LineStart {
  int line;
  int offset;
};

class Chunk {
  std::vector<uint8_t> bytecode;
  std::vector<Value> constants;
  std::vector<LineStart> lines;

  int disassembleInstruction(int offset);
  int disassembleSimpleInstruction(const std::string& name, int offset);
  int disassembleConstantInstruction(const std::string& name, int offset);
  int getLine(int offset);

public:
  void appendByte(uint8_t byte, int line);
  void disassemble();
  int addConstant(Value value);
};

#endif // CHUNK_H
