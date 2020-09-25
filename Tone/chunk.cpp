#include <iostream>

#include "chunk.h"

void Chunk::appendByte(uint8_t byte, int line) {
  bytecode.push_back(byte);

  if (!lines.empty() && lines.at(lines.size()-1).line == line) return;

  LineStart lineStart{ line, (int)(bytecode.size()-1) };
  lines.push_back(lineStart);
}

void Chunk::disassemble() {
  for (int offset = 0; offset < (int)bytecode.size();) {
    offset = disassembleInstruction(offset);
  }
}

int Chunk::disassembleInstruction(int offset) {
  std::cout << offset << " ";
  int line = getLine(offset);
  if (offset > 0 && line == getLine(offset-1)) {
    std::cout << "  | ";
  } else {
    std::cout << line << " ";
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

int Chunk::getLine(int offset) {
  if (offset < 0) return -1;

  int start = 0;
  int end = (int)(lines.size() - 1);

  for (;;) {
    int mid = (start + end) / 2;
    LineStart lineStart = lines.at(mid);
    if (offset < lineStart.offset) {
      end = mid - 1;
    } else if (mid == (int)(lines.size() - 1) || offset < lines.at(mid + 1).offset) {
      return lineStart.line;
    } else {
      start = mid + 1;
    }
  }
}
