#ifndef CHUNK_H
#define CHUNK_H

#include "value.h"

#include <map>
#include <string>
#include <vector>

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
  int disassembleByteInstruction(const std::string& name, int offset);
  int disassembleJumpInstruction(const std::string& name, int sign, int offset);
  int disassembleInvokeInstruction(const std::string& name, int offset);

public:
  enum class OpCode {
    OP_CONSTANT,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_GET_SUPER,
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
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD
  };

  void appendByte(uint8_t byte, int line);
  void disassemble(const std::string &name);
  int disassembleInstruction(int offset);
  int addConstant(const Value &value);
  int getLine(int offset);
  int getBytecodeCount();
  void setBytecodeValue(int offset, uint8_t byte);

  std::vector<uint8_t> getBytecode();
  std::vector<Value> getConstants();
};

#endif // CHUNK_H
