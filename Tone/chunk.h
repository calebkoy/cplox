#ifndef CHUNK_H
#define CHUNK_H

#include <string>
#include <vector>

#include "value.h"

enum OpCode {
  OP_CONSTANT,
  OP_RETURN,
};

class Chunk {
  std::vector<uint8_t> bytecode;
  std::vector<Value> constants;
  std::vector<int> lines;

  int disassembleInstruction(int offset);
  int disassembleSimpleInstruction(const std::string& name, int offset);
  int disassembleConstantInstruction(const std::string& name, int offset);

public:
  void appendByte(uint8_t byte, int line);
  void disassemble();
  int addConstant(Value value);
};

#endif // CHUNK_H
