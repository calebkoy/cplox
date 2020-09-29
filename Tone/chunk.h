#ifndef CHUNK_H
#define CHUNK_H

#include <map>
#include <string>
#include <vector>

#include "value.h"

enum OpCode {
  OP_CONSTANT,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
  OP_SET_GLOBAL,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN
};

struct LineStart {
  int line;
  int offset;
};

class Chunk {
  std::vector<uint8_t> bytecode;
  std::vector<Value> constants;
  std::vector<LineStart> lines;

  int disassembleSimpleInstruction(const std::string& name, int offset);
  int disassembleConstantInstruction(const std::string& name, int offset);

public:
  void appendByte(uint8_t byte, int line);
  void disassemble();
  int disassembleInstruction(int offset);
  int addConstant(Value value);
  int getLine(int offset);
  std::vector<uint8_t> getBytecode();
  std::vector<Value> getConstants();
};

#endif // CHUNK_H
