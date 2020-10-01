#include <iostream>

#include "chunk.h"
#include "functionobject.h"

// Q: is there a better way of doing this than a macro?
// Also note that this is duplicated in vm.cpp
#define AS_FUNCTION(object)  ((FunctionObject*)(object))

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
    case OP_NULL:
      return disassembleSimpleInstruction("OP_NULL", offset);
    case OP_TRUE:
      return disassembleSimpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
      return disassembleSimpleInstruction("OP_FALSE", offset);
    case OP_POP:
      return disassembleSimpleInstruction("OP_POP", offset);
    case OP_GET_LOCAL:
      return disassembleByteInstruction("OP_GET_LOCAL", offset);
    case OP_SET_LOCAL:
      return disassembleByteInstruction("OP_SET_LOCAL", offset);
    case OP_GET_GLOBAL:
      return disassembleConstantInstruction("OP_GET_GLOBAL", offset);
    case OP_DEFINE_GLOBAL:
      return disassembleConstantInstruction("OP_DEFINE_GLOBAL", offset);
    case OP_SET_GLOBAL:
      return disassembleConstantInstruction("OP_SET_GLOBAL", offset);
    case OP_GET_UPVALUE:
      return disassembleByteInstruction("OP_GET_UPVALUE", offset);
    case OP_SET_UPVALUE:
      return disassembleByteInstruction("OP_SET_UPVALUE", offset);
    case OP_GET_PROPERTY:
      return disassembleConstantInstruction("OP_GET_PROPERTY", offset);
    case OP_SET_PROPERTY:
      return disassembleConstantInstruction("OP_SET_PROPERTY", offset);
    case OP_EQUAL:
      return disassembleSimpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
      return disassembleSimpleInstruction("OP_GREATER", offset);
    case OP_LESS:
      return disassembleSimpleInstruction("OP_LESS", offset);
    case OP_ADD:
      return disassembleSimpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
      return disassembleSimpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
      return disassembleSimpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return disassembleSimpleInstruction("OP_DIVIDE", offset);
    case OP_NOT:
      return disassembleSimpleInstruction("OP_NOT", offset);
    case OP_NEGATE:
      return disassembleSimpleInstruction("OP_NEGATE", offset);
    case OP_PRINT:
      return disassembleSimpleInstruction("OP_PRINT", offset);
    case OP_JUMP:
      return disassembleJumpInstruction("OP_JUMP", 1, offset);
    case OP_JUMP_IF_FALSE:
      return disassembleJumpInstruction("OP_JUMP_IF_FALSE", 1, offset);
    case OP_LOOP:
      return disassembleJumpInstruction("OP_LOOP", -1, offset);
    case OP_CALL:
      return disassembleByteInstruction("OP_CALL", offset);
    case OP_CLOSURE: {
      offset++;
      uint8_t constant = bytecode.at(offset++);
      std::cout << "OP_CLOSURE " << +constant << " " << constants.at(constant) << '\n';

      FunctionObject* function = AS_FUNCTION(constants.at(constant).asObject());
      for (int j = 0; j < function->getUpvalueCount(); j++) {
        int isLocal = bytecode.at(offset++);
        int index = bytecode.at(offset++);

        std::cout << offset - 2 << "      |                     " << (isLocal ? "local " : "upvalue ") << index << '\n';
      }

      return offset;
    }
    case OP_CLOSE_UPVALUE:
      return disassembleSimpleInstruction("OP_CLOSE_UPVALUE", offset);
    case OP_RETURN:
      return disassembleSimpleInstruction("OP_RETURN", offset);
    case OP_CLASS:
      return disassembleConstantInstruction("OP_CLASS", offset);
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

int Chunk::disassembleByteInstruction(const std::string& name, int offset) {
  uint8_t slot = bytecode.at(offset + 1);
  std::cout << name << " " << +slot << '\n';
  return offset + 2;
}

int Chunk::disassembleJumpInstruction(const std::string& name, int sign, int offset) {
  uint16_t jump = (uint16_t)(bytecode.at(offset + 1) << 8);
  jump |= bytecode.at(offset + 2);
  std::cout << name << " " << offset << " -> " << (offset + 3 + (sign * jump)) << '\n';
  return offset + 3;
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

void Chunk::setBytecodeValue(int offset, uint8_t byte) {
  // Q: error handling?

  bytecode.at(offset) = byte;
}

int Chunk::getBytecodeCount() {
  return (int)bytecode.size(); // Q: is there anything wrong with casting to int?
}

std::vector<uint8_t> Chunk::getBytecode() {
  return bytecode;
}

std::vector<Value> Chunk::getConstants() {
  return constants;
}
