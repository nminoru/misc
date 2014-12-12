/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#include "Reg.hpp"

using namespace X64;

Reg Reg::invalid()
{
    return Reg(-1, false);
}

Reg X64::r0 ( 0, false);
Reg X64::r1 ( 1, false);
Reg X64::r2 ( 2, false);
Reg X64::r3 ( 3, false);
Reg X64::r4 ( 4, false);
Reg X64::r5 ( 5, false);
Reg X64::r6 ( 6, false);
Reg X64::r7 ( 7, false);
Reg X64::r8 ( 8, false);
Reg X64::r9 ( 9, false);
Reg X64::r10(10, false);
Reg X64::r11(11, false);
Reg X64::r12(12, false);
Reg X64::r13(13, false);
Reg X64::r14(14, false);
Reg X64::r15(15, false);
Reg X64::rax( 0, false);
Reg X64::rcx( 1, false);
Reg X64::rdx( 2, false);
Reg X64::rbx( 3, false);
Reg X64::rsp( 4, false);
Reg X64::rbp( 5, false);
Reg X64::rsi( 6, false);
Reg X64::rdi( 7, false);
Reg X64::ah ( 0, true);
Reg X64::ch ( 1, true);
Reg X64::dh ( 2, true);
Reg X64::bh ( 3, true);
Reg X64::rarg0(7, false); // rdi
Reg X64::rarg1(6, false); // rsi
Reg X64::rarg2(2, false); // rdx
Reg X64::rarg3(1, false); // rcx
Reg X64::rarg4(8, false); // r8
Reg X64::rarg5(9, false); // r9
