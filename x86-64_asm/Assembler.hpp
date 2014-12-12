/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#if !defined(__X64__ASSEMBLER_HPP__)
#define __X64__ASSEMBLER_HPP__

#include "Reg.hpp"
#include "Address.hpp"

namespace X64 {

class Label;
class Reg;
class XmmReg;
class Address;

/**
 *  Jcc, SETcc, CMOVcc に使う条件
 */
class Condition {
  
public:    
    enum T {
        Z            = 0x4, // zero                 (ZF==1)
        NZ           = 0x5, // not zero             (ZF==0)
        E            = 0x4, // equal                (ZF==1)
        NE           = 0x5, // not equal            (ZF==0)
        zero         = 0x4,
        notZero      = 0x5,
        equal        = 0x4,
        notEqual     = 0x5,

        L            = 0xC, // less                 (SF!=OF)
        LE           = 0xE, // less or equal        (ZF==1 or SF!=OF)
        G            = 0xF, // greater              (ZF==0 and SF==OF)
        GE           = 0xD, // greater or equal     (SF==OF)
        less         = 0xC,
        lessEqual    = 0xE,
        greater      = 0xF,
        greaterEqual = 0xD,

        B            = 0x2, // below                (CF==1)
        BE           = 0x6, // below or equal       (CF==1 or ZF==1)
        A            = 0x7, // above                (CF==0 and ZF==0)
        AE           = 0x3, // above or equal       (CF==0)
        below        = 0x2,
        belowEqual   = 0x6,
        above        = 0x7,
        aboveEqual   = 0x3,

        NA           = 0x6, // not above            (CF==1 or ZF==1)
        NAE          = 0x2, // not above or eqaul   (CF==1)
        NB           = 0x3, // not below            (CF==0)
        NBE          = 0x7, // not below or equal   (CF==0 and ZF==0)
        notAbove     = 0x6,
        notAboveEqual = 0x2,
        notBelow     = 0x3,
        notBelowEqual = 0x7,

        NG           = 0xE, // not greater          (ZF==1 or SF!=OF)
        NGE          = 0xC, // not greater or equal (SF!=OF)
        NL           = 0xD, // not less             (SF==OF)
        NLE          = 0xF, // not less or equal    (ZF=0 and SF==OF)
        notGreater   = 0xE,
        notGreaterEqual = 0xC,
        notLess      = 0xD,
        notLessEqual = 0xF,

        O            = 0x0, // overflow             (OF==1)
        NO           = 0x1, // not overflow         (OF==0)
        overflow     = 0x0,
        notOverflow  = 0x1,

        C            = 0x2, // carry                (CF==1)
        NC           = 0x3, // not carrry           (CF==0)
        carry        = 0x2,
        notCarry     = 0x3,
    
        P            = 0xA, // parity               (PF==1)
        NP           = 0xB, // not parity           (PF==0)
        PO           = 0xB, // parity odd           (PF==0)
        parity       = 0xA,
        notParity    = 0xB,
        parityOdd    = 0xB,

        S            = 0x8, // sign                 (SF==1)
        NS           = 0x9, // not sign             (SF==0)
        sign         = 0x8,
        notSign      = 0x9,
        negative     = 0x8,
        positive     = 0x9,
 
    };

private:
    T _t;

private:
    Condition(int i);

public:
    Condition(T t) : _t(t) {}
    int getIntValue() const { return (int)_t; }
};



/**
 *  Intel64 アセンブラ
 *
 *  - Any には Reg または Address がとれる。
 *
 *  @todo
 *  - 実装とインターフェイスの分離
 *  - 制御系命令の追加
 *  - Relocation
 */
class Assembler {

public:
    // jcc 用の分岐ヒント
    enum BranchHint {
        unknown,
        likely,
        unlikely,
    };

public:
    Assembler(uint8_t* buffer, size_t size);
    ~Assembler();

    // アセンブルを行いバッファに内容を書き出す
    void	   flush();

    // バッファ中の書き込み位置を返す。
    // uint8_t* pc() const;

    // Label をバインドする
    void     bind(Label& L);

    // アライメントの挿入 (NOPで埋める)
    void     fitAlign(int modulus);

    // バッファの現在の書き出し位置に data を配置する
    // (書き込みデータは little endian)
    void     emitData1(uint8_t data);
    void     emitData2(uint16_t data);
    void     emitData4(uint32_t data);
    void     emitData8(uint64_t data);

    // 現在のバッファの内容をディスアセンブルして表示
    void     printBuffer();

public:
    // Prefix
    Assembler& fs();
    Assembler& gs();
    Assembler& lock();
    Assembler& rep();
    Assembler& repe();
    Assembler& repne();
  
    // Moves
    void mov1(Reg dst, int imm8);
    void mov1(Address dst, int imm8);
    void mov1(Reg dst, Reg src);
    void mov1(Reg dst, Address src);
    void mov1(Address dst, Reg src);
    void mov2(Reg dst, int imm16);
    void mov2(Address dst, int imm16);
    void mov2(Reg dst, Reg src);
    void mov2(Reg dst, Address src);
    void mov2(Address dst, Reg src);
    void mov4(Reg dst, int imm32);
    void mov4(Address dst, int imm32);
    void mov4(Reg dst, Reg src);
    void mov4(Reg dst, Address src);
    void mov4(Address dst, Reg src);
    void mov8(Reg dst, int64_t imm64);
    void mov8(Address dst, int imm32);
    void mov8(Reg dst, Reg src);
    void mov8(Reg dst, Address src);
    void mov8(Address dst, Reg src);

    // Stack
    void push8(int imm32);
    void push8(Reg src);
    void push8(Address src);
    void pop8(Reg dst);
    void pop8(Address dst);

    // Sign/Zero extension
    // (レジスタは 8 バイトとして扱う) 
    void movsx1(Reg dst, Reg src);
    void movsx1(Reg dst, Address src);
    void movsx2(Reg dst, Reg src);
    void movsx2(Reg dst, Address src);
    void movsx4(Reg dst, Reg src);
    void movsx4(Reg dst, Address src);
    void movzx1(Reg dst, Reg src);
    void movzx1(Reg dst, Address src);
    void movzx2(Reg dst, Reg src);
    void movzx2(Reg dst, Address src);
    void movzx4(Reg dst, Reg src);
    void movzx4(Reg dst, Address src);

    // Arithmetics
    void add1(Reg dst, int imm8);
    void add1(Address dst, int imm8);
    void add1(Reg dst, Reg src);
    void add1(Reg dst, Address src);
    void add1(Address dst, Reg src);
    void add2(Reg dst, int imm16);
    void add2(Address dst, int imm16);
    void add2(Reg dst, Reg src);
    void add2(Reg dst, Address src);
    void add2(Address dst, Reg src);
    void add4(Reg dst, int imm32);
    void add4(Address dst, int imm32);
    void add4(Reg dst, Reg src);
    void add4(Reg dst, Address src);
    void add4(Address dst, Reg src);
    void add8(Reg dst, int imm32);
    void add8(Address dst, int imm32);
    void add8(Reg dst, Reg src);
    void add8(Reg dst, Address src);
    void add8(Address dst, Reg src);
    void adc1(Reg dst, int imm8);
    void adc1(Address dst, int imm8);
    void adc1(Reg dst, Reg src);
    void adc1(Reg dst, Address src);
    void adc1(Address dst, Reg src);
    void adc2(Reg dst, int imm16);
    void adc2(Address dst, int imm16);
    void adc2(Reg dst, Reg src);
    void adc2(Reg dst, Address src);
    void adc2(Address dst, Reg src);
    void adc4(Reg dst, int imm32);
    void adc4(Address dst, int imm32);
    void adc4(Reg dst, Reg src);
    void adc4(Reg dst, Address src);
    void adc4(Address dst, Reg src);
    void adc8(Reg dst, int imm32);
    void adc8(Address dst, int imm32);
    void adc8(Reg dst, Reg src);
    void adc8(Reg dst, Address src);
    void adc8(Address dst, Reg src);
    void sub1(Reg dst, int imm8);
    void sub1(Address dst, int imm8);
    void sub1(Reg dst, Reg src);
    void sub1(Reg dst, Address src);
    void sub1(Address dst, Reg src);
    void sub2(Reg dst, int imm16);
    void sub2(Address dst, int imm16);
    void sub2(Reg dst, Reg src);
    void sub2(Reg dst, Address src);
    void sub2(Address dst, Reg src);
    void sub4(Reg dst, int imm32);
    void sub4(Address dst, int imm32);
    void sub4(Reg dst, Reg src);
    void sub4(Reg dst, Address src);
    void sub4(Address dst, Reg src);
    void sub8(Reg dst, int imm32);
    void sub8(Address dst, int imm32);
    void sub8(Reg dst, Reg src);
    void sub8(Reg dst, Address src);
    void sub8(Address dst, Reg src);
    void sbb1(Reg dst, int imm8);
    void sbb1(Address dst, int imm8);
    void sbb1(Reg dst, Reg src);
    void sbb1(Reg dst, Address src);
    void sbb1(Address dst, Reg src);
    void sbb2(Reg dst, int imm16);
    void sbb2(Address dst, int imm16);
    void sbb2(Reg dst, Reg src);
    void sbb2(Reg dst, Address src);
    void sbb2(Address dst, Reg src);
    void sbb4(Reg dst, int imm32);
    void sbb4(Address dst, int imm32);
    void sbb4(Reg dst, Reg src);
    void sbb4(Reg dst, Address src);
    void sbb4(Address dst, Reg src);
    void sbb8(Reg dst, int imm32);
    void sbb8(Address dst, int imm32);
    void sbb8(Reg dst, Reg src);
    void sbb8(Reg dst, Address src);
    void sbb8(Address dst, Reg src);
    void imul1(Reg dst);
    void imul1(Address dst);
    void imul2(Reg dst);
    void imul2(Address dst);
    void imul2(Reg dst, Reg src);
    void imul2(Reg dst, Address src);
    void imul2(Reg dst, Reg src, int imm16);
    void imul2(Reg dst, Address src, int imm16);
    void imul4(Reg dst);
    void imul4(Address dst);
    void imul4(Reg dst, Reg src);
    void imul4(Reg dst, Address src);
    void imul4(Reg dst, Reg src, int imm32);
    void imul4(Reg dst, Address src, int imm32);
    void imul8(Reg dst);
    void imul8(Address dst);
    void imul8(Reg dst, Reg src);
    void imul8(Reg dst, Address src);
    void imul8(Reg dst, Reg src, int imm32);
    void imul8(Reg dst, Address src, int imm32);
    void mul1(Reg dst);
    void mul1(Address dst);
    void mul2(Reg dst);
    void mul2(Address dst);
    void mul4(Reg dst);
    void mul4(Address dst);
    void mul8(Reg dst);
    void mul8(Address dst);
    void idiv1(Reg dst);
    void idiv1(Address dst);
    void idiv2(Reg dst);
    void idiv2(Address dst);
    void idiv4(Reg dst);
    void idiv4(Address dst);
    void idiv8(Reg dst);
    void idiv8(Address dst);
    void div1(Reg dst);
    void div1(Address dst);
    void div2(Reg dst);
    void div2(Address dst);
    void div4(Reg dst);
    void div4(Address dst);
    void div8(Reg dst);
    void div8(Address dst);
    void neg1(Reg dst);
    void neg1(Address dst);
    void neg2(Reg dst);
    void neg2(Address dst);
    void neg4(Reg dst);
    void neg4(Address dst);
    void neg8(Reg dst);
    void neg8(Address dst);
    void inc1(Reg dst);
    void inc1(Address dst);
    void inc2(Reg dst);
    void inc2(Address dst);
    void inc4(Reg dst);
    void inc4(Address dst);
    void inc8(Reg dst);
    void inc8(Address dst);
    void dec1(Reg dst);
    void dec1(Address dst);
    void dec2(Reg dst);
    void dec2(Address dst);
    void dec4(Reg dst);
    void dec4(Address dst);
    void dec8(Reg dst);
    void dec8(Address dst);

    // Compare
    void cmp1(Reg src1, int imm8);
    void cmp1(Address src1, int imm8);
    void cmp1(Reg src1, Reg src2);
    void cmp1(Address src1, Reg src2);
    void cmp1(Reg src1, Address src2);
    void cmp2(Reg src1, int imm16);
    void cmp2(Address src1, int imm16);
    void cmp2(Reg src1, Reg src2);
    void cmp2(Address src1, Reg src2);
    void cmp2(Reg src1, Address src2);
    void cmp4(Reg src1, int imm32);
    void cmp4(Address src1, int imm32);
    void cmp4(Reg src1, Reg src2);
    void cmp4(Address src1, Reg src2);
    void cmp4(Reg src1, Address src2);
    void cmp8(Reg src1, int imm32);
    void cmp8(Address src1, int imm32);
    void cmp8(Reg src1, Reg src2);
    void cmp8(Address src1, Reg src2);
    void cmp8(Reg src1, Address src2);
    void test1(Reg src1, int imm8);
    void test1(Address src1, int imm8);
    void test1(Reg src1, Reg src2);
    void test1(Address src1, Reg src2);
    void test2(Reg src1, int imm16);
    void test2(Address src1, int imm16);
    void test2(Reg src1, Reg src2);
    void test2(Address src1, Reg src2);
    void test4(Reg src1, int imm32);
    void test4(Address src1, int imm32);
    void test4(Reg src1, Reg src2);
    void test4(Address src1, Reg src2);
    void test8(Reg src1, int imm32);
    void test8(Address src1, int imm32);
    void test8(Reg src1, Reg src2);
    void test8(Address src1, Reg src2);

    // Logical instructions
    void and1(Reg dst, int imm8);
    void and1(Address dst, int imm8);
    void and1(Reg dst, Reg src);
    void and1(Address dst, Reg src);
    void and1(Reg dst, Address src);
    void and2(Reg dst, int imm16);
    void and2(Address dst, int imm16);
    void and2(Reg dst, Reg src);
    void and2(Address dst, Reg src);
    void and2(Reg dst, Address src);
    void and4(Reg dst, int imm32);
    void and4(Address dst, int imm32);
    void and4(Reg dst, Reg src);
    void and4(Address dst, Reg src);
    void and4(Reg dst, Address src);
    void and8(Reg dst, int imm32);
    void and8(Address dst, int imm32);
    void and8(Reg dst, Reg src);
    void and8(Address dst, Reg src);
    void and8(Reg dst, Address src);
    void or1(Reg dst, int imm8);
    void or1(Address dst, int imm8);
    void or1(Reg dst, Reg src);
    void or1(Address dst, Reg src);
    void or1(Reg dst, Address src);
    void or2(Reg dst, int imm16);
    void or2(Address dst, int imm16);
    void or2(Reg dst, Reg src);
    void or2(Address dst, Reg src);
    void or2(Reg dst, Address src);
    void or4(Reg dst, int imm32);
    void or4(Address dst, int imm32);
    void or4(Reg dst, Reg src);
    void or4(Address dst, Reg src);
    void or4(Reg dst, Address src);
    void or8(Reg dst, int imm32);
    void or8(Address dst, int imm32);
    void or8(Reg dst, Reg src);
    void or8(Address dst, Reg src);
    void or8(Reg dst, Address src);
    void xor1(Reg dst, int imm8);
    void xor1(Address dst, int imm8);
    void xor1(Reg dst, Reg src);
    void xor1(Address dst, Reg src);
    void xor1(Reg dst, Address src);
    void xor2(Reg dst, int imm16);
    void xor2(Address dst, int imm16);
    void xor2(Reg dst, Reg src);
    void xor2(Address dst, Reg src);
    void xor2(Reg dst, Address src);
    void xor4(Reg dst, int imm32);
    void xor4(Address dst, int imm32);
    void xor4(Reg dst, Reg src);
    void xor4(Address dst, Reg src);
    void xor4(Reg dst, Address src);
    void xor8(Reg dst, int imm32);
    void xor8(Address dst, int imm32);
    void xor8(Reg dst, Reg src);
    void xor8(Address dst, Reg src);
    void xor8(Reg dst, Address src);
    void not1(Reg dst);
    void not1(Address dst);
    void not2(Reg dst);
    void not2(Address dst);
    void not4(Reg dst);
    void not4(Address dst);
    void not8(Reg dst);
    void not8(Address dst);

    // Shift & Rotation
    void sar1(Reg dst, int shift);
    void sar1(Address dst, int shift);
    void sar1(Reg dst);
    void sar1(Address dst);
    void sar2(Reg dst, int shift);
    void sar2(Address dst, int shift);
    void sar2(Reg dst);
    void sar2(Address dst);
    void sar4(Reg dst, int shift);
    void sar4(Address dst, int shift);
    void sar4(Reg dst);
    void sar4(Address dst);
    void sar8(Reg dst, int shift);
    void sar8(Address dst, int shift);
    void sar8(Reg dst);
    void sar8(Address dst);
    void shr1(Reg dst, int shift);
    void shr1(Address dst, int shift);
    void shr1(Reg dst);
    void shr1(Address dst);
    void shr2(Reg dst, int shift);
    void shr2(Address dst, int shift);
    void shr2(Reg dst);
    void shr2(Address dst);
    void shr4(Reg dst, int shift);
    void shr4(Address dst, int shift);
    void shr4(Reg dst);
    void shr4(Address dst);
    void shr8(Reg dst, int shift);
    void shr8(Address dst, int shift);
    void shr8(Reg dst);
    void shr8(Address dst);
    void shl1(Reg dst, int shift);
    void shl1(Address dst, int shift);
    void shl1(Reg dst);
    void shl1(Address dst);
    void shl2(Reg dst, int shift);
    void shl2(Address dst, int shift);
    void shl2(Reg dst);
    void shl2(Address dst);
    void shl4(Reg dst, int shift);
    void shl4(Address dst, int shift);
    void shl4(Reg dst);
    void shl4(Address dst);
    void shl8(Reg dst, int shift);
    void shl8(Address dst, int shift);
    void shl8(Reg dst);
    void shl8(Address dst);
    void rcl1(Reg dst, int shift);
    void rcl1(Address dst, int shift);
    void rcl1(Reg dst);
    void rcl1(Address dst);
    void rcl2(Reg dst, int shift);
    void rcl2(Address dst, int shift);
    void rcl2(Reg dst);
    void rcl2(Address dst);
    void rcl4(Reg dst, int shift);
    void rcl4(Address dst, int shift);
    void rcl4(Reg dst);
    void rcl4(Address dst);
    void rcl8(Reg dst, int shift);
    void rcl8(Address dst, int shift);
    void rcl8(Reg dst);
    void rcl8(Address dst);
    void rcr1(Reg dst, int shift);
    void rcr1(Address dst, int shift);
    void rcr1(Reg dst);
    void rcr1(Address dst);
    void rcr2(Reg dst, int shift);
    void rcr2(Address dst, int shift);
    void rcr2(Reg dst);
    void rcr2(Address dst);
    void rcr4(Reg dst, int shift);
    void rcr4(Address dst, int shift);
    void rcr4(Reg dst);
    void rcr4(Address dst);
    void rcr8(Reg dst, int shift);
    void rcr8(Address dst, int shift);
    void rcr8(Reg dst);
    void rcr8(Address dst);
    void rol1(Reg dst, int shift);
    void rol1(Address dst, int shift);
    void rol1(Reg dst);
    void rol1(Address dst);
    void rol2(Reg dst, int shift);
    void rol2(Address dst, int shift);
    void rol2(Reg dst);
    void rol2(Address dst);
    void rol4(Reg dst, int shift);
    void rol4(Address dst, int shift);
    void rol4(Reg dst);
    void rol4(Address dst);
    void rol8(Reg dst, int shift);
    void rol8(Address dst, int shift);
    void rol8(Reg dst);
    void rol8(Address dst);
    void ror1(Reg dst, int shift);
    void ror1(Address dst, int shift);
    void ror1(Reg dst);
    void ror1(Address dst);
    void ror2(Reg dst, int shift);
    void ror2(Address dst, int shift);
    void ror2(Reg dst);
    void ror2(Address dst);
    void ror4(Reg dst, int shift);
    void ror4(Address dst, int shift);
    void ror4(Reg dst);
    void ror4(Address dst);
    void ror8(Reg dst, int shift);
    void ror8(Address dst, int shift);
    void ror8(Reg dst);
    void ror8(Address dst);
    void shrd2(Reg dst, Reg src, int imm8);
    void shrd2(Address dst, Reg src, int imm8);
    void shrd2(Reg dst, Reg src);
    void shrd2(Address dst, Reg src);
    void shrd4(Reg dst, Reg src, int imm8);
    void shrd4(Address dst, Reg src, int imm8);
    void shrd4(Reg dst, Reg src);
    void shrd4(Address dst, Reg src);
    void shrd8(Reg dst, Reg src, int imm8);
    void shrd8(Address dst, Reg src, int imm8);
    void shrd8(Reg dst, Reg src);
    void shrd8(Address dst, Reg src);
    void shld2(Reg dst, Reg src, int imm8);
    void shld2(Address dst, Reg src, int imm8);
    void shld2(Reg dst, Reg src);
    void shld2(Address dst, Reg src);
    void shld4(Reg dst, Reg src, int imm8);
    void shld4(Address dst, Reg src, int imm8);
    void shld4(Reg dst, Reg src);
    void shld4(Address dst, Reg src);
    void shld8(Reg dst, Reg src, int imm8);
    void shld8(Address dst, Reg src, int imm8);
    void shld8(Reg dst, Reg src);
    void shld8(Address dst, Reg src);

    // Bit operation
    void bt2(Reg dst, int imm8);
    void bt2(Address dst, int imm8);
    void bt2(Reg dst, Reg src);
    void bt2(Address dst, Reg src);
    void bt4(Reg dst, int imm8);
    void bt4(Address dst, int imm8);
    void bt4(Reg dst, Reg src);
    void bt4(Address dst, Reg src);
    void bt8(Reg dst, int imm8);
    void bt8(Address dst, int imm8);
    void bt8(Reg dst, Reg src);
    void bt8(Address dst, Reg src);
    void bts2(Reg dst, int imm8);
    void bts2(Address dst, int imm8);
    void bts2(Reg dst, Reg src);
    void bts2(Address dst, Reg src);
    void bts4(Reg dst, int imm8);
    void bts4(Address dst, int imm8);
    void bts4(Reg dst, Reg src);
    void bts4(Address dst, Reg src);
    void bts8(Reg dst, int imm8);
    void bts8(Address dst, int imm8);
    void bts8(Reg dst, Reg src);
    void bts8(Address dst, Reg src);
    void btr2(Reg dst, int imm8);
    void btr2(Address dst, int imm8);
    void btr2(Reg dst, Reg src);
    void btr2(Address dst, Reg src);
    void btr4(Reg dst, int imm8);
    void btr4(Address dst, int imm8);
    void btr4(Reg dst, Reg src);
    void btr4(Address dst, Reg src);
    void btr8(Reg dst, int imm8);
    void btr8(Address dst, int imm8);
    void btr8(Reg dst, Reg src);
    void btr8(Address dst, Reg src);
    void btc2(Reg dst, int imm8);
    void btc2(Address dst, int imm8);
    void btc2(Reg dst, Reg src);
    void btc2(Address dst, Reg src);
    void btc4(Reg dst, int imm8);
    void btc4(Address dst, int imm8);
    void btc4(Reg dst, Reg src);
    void btc4(Address dst, Reg src);
    void btc8(Reg dst, int imm8);
    void btc8(Address dst, int imm8);
    void btc8(Reg dst, Reg src);
    void btc8(Address dst, Reg src);
    void bsf2(Reg dst, Reg src);
    void bsf2(Reg dst, Address src);
    void bsf4(Reg dst, Reg src);
    void bsf4(Reg dst, Address src);
    void bsf8(Reg dst, Reg src);
    void bsf8(Reg dst, Address src);
    void bsr2(Reg dst, Reg src);
    void bsr2(Reg dst, Address src);
    void bsr4(Reg dst, Reg src);
    void bsr4(Reg dst, Address src);
    void bsr8(Reg dst, Reg src);
    void bsr8(Reg dst, Address src);

    // Strings 
    void movs1();
    void movs2();
    void movs4();
    void movs8();
    void cmps1();
    void cmps2();
    void cmps4();
    void cmps8();
    void scas1();
    void scas2();
    void scas4();
    void scas8();
    void loads1();
    void loads2();
    void loads4();
    void loads8();
    void stos1();
    void stos2();
    void stos4();
    void stos8();

    // Load effective address
    void lea2(Reg dst, Address address);
    void lea4(Reg dst, Address address);
    void lea8(Reg dst, Address address);

    // Atomic memory operation
    void xadd1(Reg dst, Reg src);
    void xadd1(Address dst, Reg src);
    void xadd2(Reg dst, Reg src);
    void xadd2(Address dst, Reg src);
    void xadd4(Reg dst, Reg src);
    void xadd4(Address dst, Reg src);
    void xadd8(Reg dst, Reg src);
    void xadd8(Address dst, Reg src);
    void xchg1(Reg dst, Reg src);
    void xchg1(Address dst, Reg src);
    void xchg2(Reg dst, Reg src);
    void xchg2(Address dst, Reg src);
    void xchg4(Reg dst, Reg src);
    void xchg4(Address dst, Reg src);
    void xchg8(Reg dst, Reg src);
    void xchg8(Address dst, Reg src);
    void cmpxchg1(Reg dst, Reg src);
    void cmpxchg1(Address dst, Reg src);
    void cmpxchg2(Reg dst, Reg src);
    void cmpxchg2(Address dst, Reg src);
    void cmpxchg4(Reg dst, Reg src);
    void cmpxchg4(Address dst, Reg src);
    void cmpxchg8(Reg dst, Reg src);
    void cmpxchg8(Address dst, Reg src);
    void cmpxchg8b(Address dst);
    void cmpxchg16(Address dst);

    // Endian convert
    void bswap2(Reg dst) { ror2(dst, 8); }
    void bswap4(Reg dst);
    void bswap8(Reg dst);

    // Flag control
    void stc();
    void clc();
    void cmc();
    void std();
    void cld();
    void lahf();
    void sahf();
    void pushf();
    void popf();

    void nop();
    void ud2();
    void hlt();
    void interrupt(int imm8); // int

    void clflush(Address src);
    void lfence();
    void sfence();
    void mfence();

    void cpuid();  

    // Predicator
    void cmov2(Condition cc, Reg dst, Reg src);
    void cmov2(Condition cc, Reg dst, Address src);
    void cmov4(Condition cc, Reg dst, Reg src);
    void cmov4(Condition cc, Reg dst, Address src);
    void cmov8(Condition cc, Reg dst, Reg src);
    void cmov8(Condition cc, Reg dst, Address src);
    void setcc1(Condition cc, Reg dst);
    void setcc1(Condition cc, Address dst);

    // Control
    //
    // void call(Reg reg);
    // void call(Address src);
    void ret(int imm16);
  
    // void jmp(Label& L);
    // void jcc(Condition cc, Label& L, BranchHint hint = unknown);
    // void jcxz2(Label& L);
    // void jcxz4(Label& L);
    // void jcxz8(Label& L);

    // アドレスサイズプレフィックスをどうするか？
    // void loop(Label& L);
    // void loope(Label& L);
    // void loopne(Label& L);
    // void loopnz(Label& L);

    // void mov4(Reg dst, Label& L);
    // void mov8(Reg dst, Label& L);
    // void lea8(Reg dst, Label& L);

private:
    /**
     *  flush するまでの命令の内部形式
     */
    struct Inst {
        uint8_t buffer[16];
        int     size;
        int     reloc_pos;
    };

    uint8_t* _buffer;
    size_t   _pos;
    size_t   _size;

    struct {
        // オペランドサイズ
        int      _size;

        // Prefix
        uint8_t  _group1; // LOCK(F0H), F2H(REPNZ/REPNZ), F3H(REP or REPE)
        uint8_t  _group2; // FS(64H), GS(65H) 分岐ヒント(2EH, 3EH)
        uint8_t  _group3; // 66H
        uint8_t  _group4; // 67H
        uint8_t  _rex;

        uint64_t _opcode;

        bool     _useModRM;
        int      _ModRMmod;
        int      _ModRMreg;
        int      _ModRM_RM;

        bool     _useSIB;
        int      _sibScale;
        int      _sibIndex;
        int      _sibBase;

        bool     _useDisp;
        int32_t  _disp;
        int      _dispSize;

        int      _imm_size;
        int64_t  _imm;

        // AH, BH, CH, DH の使用をチェック
        bool     _useAHintoOpfield;
        bool     _useAHintoModRMreg;
        bool     _useAHintoModRMRM;
    } _cur;

    void writeBegin();
    void writeEnd();

    void writeByte(uint8_t data);
    void writeWord(uint16_t data);
    void writeDWord(uint32_t data);
    void writeQWord(uint64_t data);

    void setSize(int size);  
    void setPrefix66();
    void setPrefixRexW();
    void setOpcode(uint64_t opcode);
    void setOpfield(int id);
    void setModRMreg(int id);
    void setModRMreg(const Reg& reg);
    void setOpfield(const Reg& reg);
    void setOperand(const Reg& reg);
    void setOperand(const Address& address);
    void setImm8(int imm8);
    void setImm16(int imm16);
    void setImm32(int imm32);
    void setImm64(int64_t imm64);

    void checkUpperHalfReg();
};


}

#endif /* __X64__ASSEMBLER_HPP__ */
