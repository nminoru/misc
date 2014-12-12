#include <stdio.h>
#include <string.h>

#include "Assembler.hpp"

using namespace X64;

int main(int argc, char** argv)
{
  uint8_t buffer[256];

  memset(buffer, 0, sizeof(buffer));

  Assembler masm(buffer, 256);

  masm.bt4   (Address(rax), 7);
  masm.mov4  (rax, Address(rbx, -4));
  masm.mov4  (rax, Address(rcx, rbx, Scale::times8, -4));
  masm.bswap4(rax);
  masm.cmov4 (Condition::Z, rcx, rax);
  masm.add4  (Address(rcx, 128), 40);
  masm.setcc1(Condition::E, ah);
  masm.mov4  (rbp, 0x12345678);
  masm.lock().fs().add4 (Address(rsp, 128), 40);

  masm.ror4 (Address(rbp, +16));

  masm.shl2(rbx, 1);
  masm.shl2(rbx, 2);

  masm.mov4 (rax, Address(rsp));
  // masm.mov4 (rax, Address(r12));
  masm.mov4 (rax, Address(rbp));
  // masm.mov4 (rax, Address(r13));
  masm.mov4 (rax, Address(rsp, rax, Scale::times1));
  // masm.mov4 (rax, Address(r12, rax, Scale::times1));
  masm.mov4 (rax, Address(rbp, rax, Scale::times1));
  // masm.mov4 (rax, Address(r13, rax, Scale::times1));

  masm.cmp4 (rax, rax);
  

#if 0
  Lable label0;

  // 定数データから 0xCafeBabe を読み込む
  masm.mov8 (rax, Address(label0));

  masm.ret();

  
  masm.fitAlign(16);

  masm.bind(label0);
  // 単なるデータの配置
  masm.emitData8(0xCafeBabe);
  masm.emitData8(0xDeadBeaf);

//
//  movq  .LABEL, rax;
//  //
//  ret 
//  .align 16
// .LABEL:
//  .long  0xCafeBabe
//  .long  0xDeadBeaf

#endif  

  masm.printBuffer();
  
  return 0;
}
