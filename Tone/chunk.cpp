#include <iostream>

#include "chunk.h"

void Chunk::appendByte(uint8_t byte, int line) {
  bytecode.push_back(byte);
  //lines.push_back(line);

  // if lines has key `line`, increment the value
  std::map<int, int>::iterator it = lines.find(line);
  if (it != lines.end()) {
    ++it->second;
  } else {
    lines.insert(std::make_pair(line, 1));
  }
  // o/w add the key w/ a value of 1
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

int Chunk::getLine(int instructionIndex) {
  if (instructionIndex < 0) return -1;

  auto it{ lines.cbegin() };
  int valueSum{ 0 };
  while (it != lines.cend()) {
    valueSum += it->second;
    if (valueSum >= instructionIndex+1) return it->first;
  }

  return -1;
}
