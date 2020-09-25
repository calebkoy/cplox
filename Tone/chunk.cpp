#include <iostream>

#include "chunk.h"

void Chunk::appendByte(uint8_t byte, int line) {
  bytecode.push_back(byte);
  lines.push_back(line);
}

void Chunk::disassemble() {
  for (int offset = 0; offset < (int)bytecode.size();) {
    offset = disassembleInstruction(offset);
  }
}

int Chunk::disassembleInstruction(int offset) {
  std::cout << offset << " ";
  if (offset > 0 && lines.at(offset) == lines.at(offset-1)) {
    std::cout << "  | ";
  } else {
    std::cout << lines.at(offset) << " ";
  }

  uint8_t instruction = bytecode.at(offset);
  switch (instruction) {
    case OP_CONSTANT:
      return disassembleConstantInstruction("OP_CONSTANT", offset);
    case OP_RETURN:
      return disassembleSimpleInstruction("OP_RETURN", offset);
    default:
      std::cout << "Unknown opcode " << instruction << '\n';
      return offset + 1;
  }
}

int Chunk::disassembleSimpleInstruction(const std::string& name, int offset) {
  std::cout << name << '\n';
  return offset + 1;
}

int Chunk::addConstant(Value value) {
  constants.push_back(value);
  return (int)(constants.size() - 1);
}

int Chunk::disassembleConstantInstruction(const std::string& name, int offset) {
  uint8_t constant = bytecode.at(offset + 1);
  std::cout << name << " " << +constant << " '" << constants.at(constant) << "'\n";
  return offset + 2;
}
