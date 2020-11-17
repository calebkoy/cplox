#include "chunk.h"
#include "functionobject.h"

#include <iostream>
#include <memory>

void Chunk::appendByte(uint8_t byte, int line) {
  bytecode.push_back(byte);
  if (!lines.empty() && lines.at(lines.size()-1).line == line) return;
  lines.push_back({ line, static_cast<int>(bytecode.size()-1) });
}

void Chunk::disassemble(const std::string &name) {
  std::cout << "== " << name << " ==\n";
  for (int offset = 0; offset < static_cast<int>(bytecode.size()); ) {
    offset = disassembleInstruction(offset);
  }
}

int Chunk::disassembleInstruction(int offset) {
  printf("%04d ", offset);
  int line = getLine(offset);
  if (offset > 0 && line == getLine(offset-1)) {
    std::cout << "   | ";
  } else {
    printf("%4d ", line);
  }

  uint8_t instruction = bytecode.at(offset);
  switch (instruction) {
    case static_cast<unsigned int>(OpCode::OP_CONSTANT):
      return disassembleConstantInstruction("OP_CONSTANT", offset);
    case static_cast<unsigned int>(OpCode::OP_NULL):
      return disassembleSimpleInstruction("OP_NULL", offset);
    case static_cast<unsigned int>(OpCode::OP_TRUE):
      return disassembleSimpleInstruction("OP_TRUE", offset);
    case static_cast<unsigned int>(OpCode::OP_FALSE):
      return disassembleSimpleInstruction("OP_FALSE", offset);
    case static_cast<unsigned int>(OpCode::OP_POP):
      return disassembleSimpleInstruction("OP_POP", offset);
    case static_cast<unsigned int>(OpCode::OP_GET_LOCAL):
      return disassembleByteInstruction("OP_GET_LOCAL", offset);
    case static_cast<unsigned int>(OpCode::OP_SET_LOCAL):
      return disassembleByteInstruction("OP_SET_LOCAL", offset);
    case static_cast<unsigned int>(OpCode::OP_GET_GLOBAL):
      return disassembleConstantInstruction("OP_GET_GLOBAL", offset);
    case static_cast<unsigned int>(OpCode::OP_DEFINE_GLOBAL):
      return disassembleConstantInstruction("OP_DEFINE_GLOBAL", offset);
    case static_cast<unsigned int>(OpCode::OP_SET_GLOBAL):
      return disassembleConstantInstruction("OP_SET_GLOBAL", offset);
    case static_cast<unsigned int>(OpCode::OP_GET_UPVALUE):
      return disassembleByteInstruction("OP_GET_UPVALUE", offset);
    case static_cast<unsigned int>(OpCode::OP_SET_UPVALUE):
      return disassembleByteInstruction("OP_SET_UPVALUE", offset);
    case static_cast<unsigned int>(OpCode::OP_GET_PROPERTY):
      return disassembleConstantInstruction("OP_GET_PROPERTY", offset);
    case static_cast<unsigned int>(OpCode::OP_SET_PROPERTY):
      return disassembleConstantInstruction("OP_SET_PROPERTY", offset);
    case static_cast<unsigned int>(OpCode::OP_GET_SUPER):
      return disassembleConstantInstruction("OP_GET_SUPER", offset);
    case static_cast<unsigned int>(OpCode::OP_EQUAL):
      return disassembleSimpleInstruction("OP_EQUAL", offset);
    case static_cast<unsigned int>(OpCode::OP_GREATER):
      return disassembleSimpleInstruction("OP_GREATER", offset);
    case static_cast<unsigned int>(OpCode::OP_LESS):
      return disassembleSimpleInstruction("OP_LESS", offset);
    case static_cast<unsigned int>(OpCode::OP_ADD):
      return disassembleSimpleInstruction("OP_ADD", offset);
    case static_cast<unsigned int>(OpCode::OP_SUBTRACT):
      return disassembleSimpleInstruction("OP_SUBTRACT", offset);
    case static_cast<unsigned int>(OpCode::OP_MULTIPLY):
      return disassembleSimpleInstruction("OP_MULTIPLY", offset);
    case static_cast<unsigned int>(OpCode::OP_DIVIDE):
      return disassembleSimpleInstruction("OP_DIVIDE", offset);
    case static_cast<unsigned int>(OpCode::OP_NOT):
      return disassembleSimpleInstruction("OP_NOT", offset);
    case static_cast<unsigned int>(OpCode::OP_NEGATE):
      return disassembleSimpleInstruction("OP_NEGATE", offset);
    case static_cast<unsigned int>(OpCode::OP_PRINT):
      return disassembleSimpleInstruction("OP_PRINT", offset);
    case static_cast<unsigned int>(OpCode::OP_JUMP):
      return disassembleJumpInstruction("OP_JUMP", 1, offset);
    case static_cast<unsigned int>(OpCode::OP_JUMP_IF_FALSE):
      return disassembleJumpInstruction("OP_JUMP_IF_FALSE", 1, offset);
    case static_cast<unsigned int>(OpCode::OP_LOOP):
      return disassembleJumpInstruction("OP_LOOP", -1, offset);
    case static_cast<unsigned int>(OpCode::OP_CALL):
      return disassembleByteInstruction("OP_CALL", offset);
    case static_cast<unsigned int>(OpCode::OP_INVOKE):
      return disassembleInvokeInstruction("OP_INVOKE", offset);
    case static_cast<unsigned int>(OpCode::OP_SUPER_INVOKE):
      return disassembleInvokeInstruction("OP_SUPER_INVOKE", offset);
    case static_cast<unsigned int>(OpCode::OP_CLOSURE): {
      offset++;
      uint8_t constant = bytecode.at(offset++);
      printf("%-16s %4d ", "OP_CLOSURE", constant);
      std::cout << "<fn " << constants.at(constant) << ">\n";
      auto function = std::static_pointer_cast<FunctionObject>(constants.at(constant).asObject());
      auto upvalueCount = function->getUpvalueCount();
      for (int j = 0; j < upvalueCount; j++) {
        int isLocal = bytecode.at(offset++);
        int index = bytecode.at(offset++);
        printf("%04d      |                     %s %d\n",
               offset - 2,
               isLocal ? "local" : "upvalue",
               index);
      }

      return offset;
    }
    case static_cast<unsigned int>(OpCode::OP_CLOSE_UPVALUE):
      return disassembleSimpleInstruction("OP_CLOSE_UPVALUE", offset);
    case static_cast<unsigned int>(OpCode::OP_RETURN):
      return disassembleSimpleInstruction("OP_RETURN", offset);
    case static_cast<unsigned int>(OpCode::OP_CLASS):
      return disassembleConstantInstruction("OP_CLASS", offset);
    case static_cast<unsigned int>(OpCode::OP_INHERIT):
      return disassembleSimpleInstruction("OP_INHERIT", offset);
    case static_cast<unsigned int>(OpCode::OP_METHOD):
      return disassembleConstantInstruction("OP_METHOD", offset);
    default:
      std::cout << "Unknown opcode " << +instruction << '\n';
      return offset + 1;
  }
}

int Chunk::disassembleSimpleInstruction(const std::string& name, int offset) {
  std::cout << name << '\n';
  return offset + 1;
}

int Chunk::addConstant(const Value &value) {
  constants.push_back(value);
  return static_cast<int>(constants.size() - 1);
}

int Chunk::disassembleConstantInstruction(const std::string& name, int offset) {
  uint8_t constant = bytecode.at(offset + 1);
  printf("%-16s %4d '", name.c_str(), constant);
  std::cout << constants.at(constant) << "'\n";
  return offset + 2;
}

int Chunk::disassembleByteInstruction(const std::string& name, int offset) {
  uint8_t slot = bytecode.at(offset + 1);
  printf("%-16s %4d\n", name.c_str(), slot);
  return offset + 2;
}

int Chunk::disassembleInvokeInstruction(const std::string& name, int offset) {
  uint8_t constant = bytecode.at(offset + 1);
  uint8_t argCount = bytecode.at(offset + 2);
  printf("%-16s (%d args) %4d '", name.c_str(), argCount, constant);
  std::cout << constants.at(constant) << "'\n";
  return offset + 3;
}

int Chunk::disassembleJumpInstruction(const std::string& name, int sign, int offset) {
  uint16_t jump = static_cast<uint16_t>(bytecode.at(offset + 1) << 8);
  jump |= bytecode.at(offset + 2);
  printf("%-16s %4d -> %d\n", name.c_str(), offset, offset + 3 + (sign * jump));
  return offset + 3;
}

int Chunk::getLine(int offset) {
  if (offset < 0) return -1;
  int start = 0;
  int end = static_cast<int>(lines.size() - 1);
  for (;;) {
    int mid = (start + end) / 2;
    LineStart lineStart = lines.at(mid);
    if (offset < lineStart.offset) {
      end = mid - 1;
    } else if (mid == static_cast<int>(lines.size() - 1) ||
               offset < lines.at(mid + 1).offset) {

      return lineStart.line;
    } else {
      start = mid + 1;
    }
  }
}

void Chunk::setBytecodeValue(int offset, uint8_t byte) {
  bytecode.at(offset) = byte;
}

int Chunk::getBytecodeCount() {
  return static_cast<int>(bytecode.size());
}

std::vector<uint8_t> Chunk::getBytecode() {
  return bytecode;
}

std::vector<Value> Chunk::getConstants() {
  return constants;
}
