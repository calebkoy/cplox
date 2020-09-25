#include <iostream>

#include "chunk.h"
#include "tone.h"
#include "vm.h"

int main(int argc, char *argv[]) {
  Chunk chunk;

  int constant = chunk.addConstant(1.2);
  chunk.appendByte(OP_CONSTANT, 123);
  chunk.appendByte(constant, 123);

  constant = chunk.addConstant(3.4);
  chunk.appendByte(OP_CONSTANT, 123);
  chunk.appendByte(constant, 123);

  chunk.appendByte(OP_ADD, 123);

  constant = chunk.addConstant(5.6);
  chunk.appendByte(OP_CONSTANT, 123);
  chunk.appendByte(constant, 123);

  chunk.appendByte(OP_DIVIDE, 123);
  chunk.appendByte(OP_NEGATE, 123);

  chunk.appendByte(OP_RETURN, 123);
  chunk.disassemble();

  VM vm{ chunk };
  vm.interpret();

//  Tone tone;
//
//  if (argc == 1) {
//    tone.repl();
//  } else if (argc == 2) {
//    tone.runFile(argv[1]);
//  } else {
//    std::cerr << "Expected usage: tone [path]\n";
//    exit(64);
//  }

  return 0;
}
