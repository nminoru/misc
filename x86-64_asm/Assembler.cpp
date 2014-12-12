/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Assembler.hpp"

extern "C" {
    extern int 
    disassemble_instructions(const char* start_memaddr, int length, FILE* stream);
};


using namespace X64;


Assembler::Assembler(uint8_t* buffer, size_t size)
    : _buffer(buffer), _pos(0), _size(size)
{
  
    // 初期状態
    memset(&_cur, 0, sizeof(_cur));
    _cur._ModRMmod = 3;
}

Assembler::~Assembler()
{
}

void Assembler::flush()
{
}

void Assembler::printBuffer()
{
    disassemble_instructions((const char*)_buffer, _pos, stdout);
}

void Assembler::bind(Label& L)
{
}

void Assembler::fitAlign(int modulus)
{
}

void Assembler::emitData1(uint8_t data)
{
}

void Assembler::emitData2(uint16_t data)
{
}

void Assembler::emitData4(uint32_t data)
{
}

void Assembler::emitData8(uint64_t data)
{
}

Assembler& Assembler::fs()
{
    _cur._group2 = 0x64;
    return *this;
}

Assembler& Assembler::gs()
{
    _cur._group2 = 0x65;
    return *this;
}

Assembler& Assembler::lock()
{
    _cur._group1 = 0xF0;
    return *this;
}

Assembler& Assembler::rep()
{
    _cur._group1 = 0xF3;
    return *this;
}

Assembler& Assembler::repe()
{
    _cur._group1 = 0xF3;
    return *this;
}

Assembler& Assembler::repne()
{
    _cur._group1 = 0xF2;
    return *this;
}

void Assembler::cmov2(Condition cc, Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::cmov2(Condition cc, Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::cmov4(Condition cc, Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::cmov4(Condition cc, Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::cmov8(Condition cc, Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::cmov8(Condition cc, Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0F40 | cc.getIntValue());
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::setcc1(Condition cc, Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0F90 | cc.getIntValue());
    setOperand(dst);
    writeEnd();
}

void Assembler::setcc1(Condition cc, Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0F90 | cc.getIntValue());
    setOperand(dst);
    writeEnd();
}

void Assembler::ret(int imm16)
{
    writeBegin();
    if (imm16 == 0) {
        setOpcode(0xC3);
    } else {
        setOpcode(0xC2);
        setImm16(imm16);
    }
    writeEnd();
}

/***************************************************************************/
/* 命令フィールドの生成関数                                                */
/***************************************************************************/

void Assembler::writeBegin()
{
}

void Assembler::writeEnd()
{
    int i;

    // プレフィックスの出力
    if (_cur._group1) {
        writeByte(_cur._group1);
    }
    if (_cur._group2) {
        writeByte(_cur._group2);
    }
    if (_cur._group3) {
        writeByte(_cur._group3);
    }
    if (_cur._group4) {
        writeByte(_cur._group4);
    }
    if (_cur._rex) {
        // REX プレフィックスと AH, BH, CH, DH は同時使用できない。
        assert(!_cur._useAHintoOpfield);
        assert(!_cur._useAHintoModRMreg);
        assert(!_cur._useAHintoModRMRM);
        writeByte(_cur._rex);
    }
  
    // オペコードの出力
    uint64_t opcode = _cur._opcode;
    for (i=7; i>=0; i--) {
        uint8_t byte = (uint8_t)(opcode >> (i*8));
        if (byte) {
            writeByte(byte);
            i--;
            for (; i>=0; i--) {
                uint8_t byte = (uint8_t)(opcode >> (i*8));
                writeByte(byte);
            }
        }
    }

    // ModRM の出力
    if (_cur._useModRM) {
        writeByte((_cur._ModRMmod << 6) | (_cur._ModRMreg << 3) | _cur._ModRM_RM);
    }
  
    // SIB の出力
    if (_cur._useSIB) {
        writeByte((_cur._sibScale << 6) | (_cur._sibIndex << 3) | _cur._sibBase);
    }

    // Displacement の出力
    if (_cur._useDisp) {
        if (_cur._dispSize == 1) {
            writeByte(_cur._disp);
        } else {
            writeDWord(_cur._disp);
        }
    }

    // 即値 の出力
    switch(_cur._imm_size) {
    case 1:
        writeByte(_cur._imm);
        break;
    case 2:
        writeWord(_cur._imm);
        break;
    case 4:
        writeDWord(_cur._imm);
        break;
    case 8:
        writeQWord(_cur._imm);
        break;
    }

    // 命令の実行が終了後、次の命令のために
    // ワークエリアをクリアする。
    memset(&_cur, 0, sizeof(_cur));
    _cur._ModRMmod = 3;
}

void Assembler::writeByte(uint8_t data)
{
    assert(_pos + sizeof(data) <= _size);
    *(uint8_t*)(_buffer + _pos) = data;
    _pos += sizeof(data);
}

void Assembler::writeWord(uint16_t data)
{
    assert(_pos + sizeof(data) <= _size);
    *(uint16_t*)(_buffer + _pos) = data;
    _pos += sizeof(data);
}

void Assembler::writeDWord(uint32_t data)
{
    assert(_pos + sizeof(data) <= _size);
    *(uint32_t*)(_buffer + _pos) = data;
    _pos += sizeof(data);
}

void Assembler::writeQWord(uint64_t data)
{
    assert(_pos + sizeof(data) <= _size);
    *(uint64_t*)(_buffer + _pos) = data;
    _pos += sizeof(data);
}

void Assembler::setSize(int size)
{
    _cur._size = size;
}

void Assembler::setPrefix66()
{
    _cur._group3 = 0x66;
}

void Assembler::setPrefixRexW()
{
    _cur._rex    |= 0x48; // REX.W
}

void Assembler::setOpcode(uint64_t opcode)
{
    _cur._opcode = opcode;
}

void Assembler::setOpfield(int id)
{
    assert((0 <= id) && (id < 8)); 
    _cur._opcode |= id;
}

void Assembler::setOpfield(const Reg& reg)
{
    int id = reg.getEncoding();

    // AH, BH, CH, DH は 1 バイト命令でしか使えない
    if (reg.isUpperHalf()) {
        assert(_cur._size == 1);
        _cur._useAHintoOpfield = true;
    }

    // 1 バイト命令では BPL, SPL, DIL, SIL を使う場合 REX をつける
    if (_cur._size == 1) {
        if (reg.needsRexIfByte()) {
            _cur._rex  |= 0x40; // REX
        }
    }

    if (id < 8) {
        setOpfield(id);
    } else {
        _cur._rex    |= 0x41; // REX.B
        setOpfield(id - 8);
    }
}

void Assembler::setModRMreg(int id)
{
    assert((0 <= id) && (id < 8)); 
    _cur._useModRM = true;
    _cur._ModRMreg = id;
}

void Assembler::setModRMreg(const Reg& reg)
{
    int id = reg.getEncoding();

    // AH, BH, CH, DH は 1 バイト命令でしか使えない
    if (reg.isUpperHalf()) {
        assert(_cur._size == 1);
        _cur._useAHintoModRMreg = true;
    }

    // 1 バイト命令では BPL, SPL, DIL, SIL を使う場合 REX をつける
    if (_cur._size == 1) {
        if (reg.needsRexIfByte()) {
            _cur._rex  |= 0x40; // REX
        }
    }

    if (id < 8) {
        setModRMreg(id);
    } else {
        _cur._rex    |= 0x44; // REX.R
        setModRMreg(id - 8);
    }
}

void Assembler::setOperand(const Reg& reg)
{
    _cur._useModRM = true;
    _cur._ModRMmod = 3;

    int id = reg.getEncoding();

    // AH, BH, CH, DH は 1 バイト命令でしか使えない
    if (reg.isUpperHalf()) {
        assert(_cur._size == 1);
        _cur._useAHintoModRMRM = true;
    }

    // 1 バイト命令では BPL, SPL, DIL, SIL を使う場合 REX をつける
    if (_cur._size == 1) {
        if (reg.needsRexIfByte()) {
            _cur._rex  |= 0x40; // REX
        }
    }

    if (id < 8) {
        _cur._ModRM_RM = id;
    } else {
        _cur._rex    |= 0x41; // REX.B
        _cur._ModRM_RM = id - 8;
    }
}

void Assembler::setOperand(const Address& adr)
{
    _cur._useModRM = true;

    // ディスプレイスメントの決定
    _cur._disp     = adr.getDisp();

    if (_cur._disp == 0) {
        _cur._ModRMmod = 0;
    } else if (adr.isSimm8()) {
        _cur._ModRMmod = 1;
        _cur._useDisp  = true;
        _cur._dispSize = 1;
    } else {
        _cur._ModRMmod = 2;
        _cur._useDisp  = true;
        _cur._dispSize = 4;
    }

    const Reg& baseReg  = adr.getBase();
    const Reg& indexReg = adr.getIndex();

    if (!indexReg.isInvalid()) {
        // 2レジスタ指定は SIB を使う
        goto use_sib;

    } else {
        // 1レジスタ指定の場合

        // 特殊パターン
        // - rsp と r12 がベースレジスタが出てくる場合は SIB を使う。
        // - 変位なしの rbp と r13 の指定(mod=0, r/m == 101b) は、
        //   即値が 0 の mod=01に変更する。
        if (baseReg == rsp || baseReg == r12) {
            goto use_sib;
        } else if (baseReg == rbp || baseReg == r13) {
            if (_cur._disp == 0) {
                _cur._ModRMmod = 1;
                _cur._useDisp  = true;
                _cur._dispSize = 1;
            }
        }

        // ModRM の R/M フィールド
        int baseRegId = baseReg.getEncoding();
        if (baseRegId < 8) {
            _cur._ModRM_RM = baseRegId;
        } else {
            _cur._rex    |= 0x41; // REX.B
            _cur._ModRM_RM = baseRegId - 8;
        }

        return;
    }

 use_sib:
    _cur._useSIB   = true;
    _cur._ModRM_RM = rsp.getEncoding(); // SIB の使用を意味する。

    // ベースレジスタの設定
    if ((baseReg == rbp || baseReg == r13) && _cur._ModRMmod == 0) {
        // ModRM の mod=0 の場合は rbp と r13 をベースレジスタにとれない。 
        //
        // 「インテル EM64-T ソフトウェア・デベロッパーズ・ガイド」の表1-10は
        // 間違っている。
        // 
        if (_cur._disp == 0) {
            _cur._ModRMmod = 1;
            _cur._useDisp  = true;
            _cur._dispSize = 1;
        }
    }

    int baseRegId = baseReg.getEncoding();
    if (baseRegId < 8) {
        _cur._sibBase = baseRegId;
    } else {
        _cur._rex    |= 0x41; // REX.B
        _cur._sibBase = baseRegId - 8;
    }

    // インデックスレジスタの設定

    // rsp をインデックスレジスタにできない。
    assert(indexReg != rsp);
    int indexRegId = indexReg.getEncoding();
    if (indexReg.isInvalid()) {
        _cur._sibIndex = rsp.getEncoding();
    } else if (indexRegId < 8) {
        _cur._sibIndex = indexRegId;
    } else {
        _cur._rex     |= 0x42; // REX.X
        _cur._sibIndex = indexRegId - 8;
    }

    // スケールの設定
    if (adr.getScale().isInvalid()) {
        _cur._sibScale = 0;
    } else {
        _cur._sibScale = adr.getScale().getEncoding();
    }
}

void Assembler::setImm8(int imm8)
{
    _cur._imm_size = 1;
    _cur._imm      = (int8_t)imm8;
}

void Assembler::setImm16(int imm16)
{
    _cur._imm_size = 2;
    _cur._imm      = (int16_t)imm16;
}

void Assembler::setImm32(int imm32)
{
    _cur._imm_size = 4;
    _cur._imm      = (int32_t)imm32;
}

void Assembler::setImm64(int64_t imm64)
{
    _cur._imm_size = 8;
    _cur._imm      = imm64;
}

/***************************************************************************/
/* ここより下は自動生成                                                    */
/***************************************************************************/

void Assembler::mov1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0xB0);
    setOpfield(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::mov1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setPrefix66();
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::mov1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x8A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x8A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x88);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::mov2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xB8);
    setOpfield(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::mov2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xC7);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::mov2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x89);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::mov4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0xB8);
    setOpfield(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::mov4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0xC7);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::mov4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x89);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::mov8(Reg dst, int64_t imm64)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xB8);
    setOpfield(dst);
    setImm64(imm64);
    writeEnd();
}

void Assembler::mov8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xC7);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::mov8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x8B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::mov8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x89);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::push8(int imm32)
{
    writeBegin();
    setOpcode(0x68);
    setImm32(imm32);
    writeEnd();
}

void Assembler::push8(Reg src)
{
    writeBegin();
    setOpcode(0x50);
    setOpfield(src);
    writeEnd();
}

void Assembler::push8(Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0xFF);
    setModRMreg(6);
    setOperand(src);
    writeEnd();
}

void Assembler::pop8(Reg dst)
{
    writeBegin();
    setOpcode(0x58);
    setOpfield(dst);
    writeEnd();
}

void Assembler::pop8(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0x8F);
    setOperand(dst);
    writeEnd();
}

void Assembler::movsx1(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBE);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movsx1(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBE);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movsx2(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movsx2(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movsx4(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x63);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movsx4(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x63);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx1(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB6);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx1(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB6);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx2(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB7);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx2(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB7);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx4(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB7);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::movzx4(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB7);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::add1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::add1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x02);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x02);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x00);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::add2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::add2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::add2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x01);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::add4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::add4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::add4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x01);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::add8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::add8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::add8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x03);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::add8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x01);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::adc1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(2);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::adc1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(2);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::adc1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x12);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x12);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x10);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::adc2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::adc2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::adc2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x11);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::adc4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::adc4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::adc4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x11);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::adc8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::adc8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(2);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::adc8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x13);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::adc8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x11);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sub1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::sub1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::sub1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x2A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x2A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x28);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sub2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::sub2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::sub2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x29);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sub4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sub4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sub4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x29);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sub8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sub8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(5);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sub8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x2B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sub8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x29);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sbb1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(3);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::sbb1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(3);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::sbb1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x1A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x1A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x18);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sbb2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::sbb2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::sbb2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x19);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sbb4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sbb4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sbb4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x19);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::sbb8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sbb8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(3);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::sbb8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x1B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::sbb8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x19);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul2(Reg dst, Reg src, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm16(imm16);
    writeEnd();
}

void Assembler::imul2(Reg dst, Address src, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm16(imm16);
    writeEnd();
}

void Assembler::imul4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul4(Reg dst, Reg src, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm32(imm32);
    writeEnd();
}

void Assembler::imul4(Reg dst, Address src, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm32(imm32);
    writeEnd();
}

void Assembler::imul8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::imul8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAF);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::imul8(Reg dst, Reg src, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm32(imm32);
    writeEnd();
}

void Assembler::imul8(Reg dst, Address src, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x69);
    setModRMreg(dst);
    setOperand(src);
    setImm32(imm32);
    writeEnd();
}

void Assembler::mul1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::mul8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::idiv8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::div1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::div8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(6);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::neg8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xFE);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xFE);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::inc8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xFF);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xFE);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xFE);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::dec8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xFF);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmp1(Reg src1, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(7);
    setOperand(src1);
    setImm8(imm8);
    writeEnd();
}

void Assembler::cmp1(Address src1, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(7);
    setOperand(src1);
    setImm8(imm8);
    writeEnd();
}

void Assembler::cmp1(Reg src1, Reg src2)
{
    writeBegin();
    setSize(1);
    setOpcode(0x38);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp1(Address src1, Reg src2)
{
    writeBegin();
    setSize(1);
    setOpcode(0x38);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp1(Reg src1, Address src2)
{
    writeBegin();
    setSize(1);
    setOpcode(0x3A);
    setModRMreg(src1);
    setOperand(src2);
    writeEnd();
}

void Assembler::cmp2(Reg src1, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm16(imm16);
    writeEnd();
}

void Assembler::cmp2(Address src1, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm16(imm16);
    writeEnd();
}

void Assembler::cmp2(Reg src1, Reg src2)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp2(Address src1, Reg src2)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp2(Reg src1, Address src2)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x3B);
    setModRMreg(src1);
    setOperand(src2);
    writeEnd();
}

void Assembler::cmp4(Reg src1, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::cmp4(Address src1, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::cmp4(Reg src1, Reg src2)
{
    writeBegin();
    setSize(4);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp4(Address src1, Reg src2)
{
    writeBegin();
    setSize(4);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp4(Reg src1, Address src2)
{
    writeBegin();
    setSize(4);
    setOpcode(0x3B);
    setModRMreg(src1);
    setOperand(src2);
    writeEnd();
}

void Assembler::cmp8(Reg src1, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::cmp8(Address src1, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::cmp8(Reg src1, Reg src2)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp8(Address src1, Reg src2)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x39);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::cmp8(Reg src1, Address src2)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x3B);
    setModRMreg(src1);
    setOperand(src2);
    writeEnd();
}

void Assembler::and1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::and1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::and1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x20);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x20);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x22);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::and2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::and2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::and2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x23);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::and4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::and4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::and4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x23);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::and8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::and8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(4);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::and8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x21);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::and8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x23);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::or1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(1);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::or1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(1);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::or1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x08);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x08);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0A);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::or2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::or2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::or2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::or4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::or4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::or4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::or8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::or8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(1);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::or8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x09);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::or8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0B);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::xor1(Reg dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::xor1(Address dst, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0x80);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::xor1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x30);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x30);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor1(Reg dst, Address src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x32);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::xor2(Reg dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::xor2(Address dst, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm16(imm16);
    writeEnd();
}

void Assembler::xor2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x33);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::xor4(Reg dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::xor4(Address dst, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::xor4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x33);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::xor8(Reg dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::xor8(Address dst, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x81);
    setModRMreg(6);
    setOperand(dst);
    setImm32(imm32);
    writeEnd();
}

void Assembler::xor8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x31);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xor8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x33);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::not1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::not8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(7);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(7);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::sar8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::sar8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(7);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(5);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(5);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shr8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shr8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(5);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(4);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(4);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::shl8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::shl8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(4);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(2);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(2);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcl8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcl8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(3);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(3);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rcr8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rcr8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(0);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::rol8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::rol8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror1(Reg dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror1(Address dst, int shift)
{
    writeBegin();
    setSize(1);
    if (shift == 1) {
        setOpcode(0xD0);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC0);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror1(Reg dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror1(Address dst)
{
    writeBegin();
    setSize(1);
    setOpcode(0xD2);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror2(Reg dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror2(Address dst, int shift)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror2(Reg dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror2(Address dst)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror4(Reg dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror4(Address dst, int shift)
{
    writeBegin();
    setSize(4);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror4(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror8(Reg dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror8(Address dst, int shift)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    if (shift == 1) {
        setOpcode(0xD1);
        setModRMreg(1);
        setOperand(dst);
    } else {
        setOpcode(0xC1);
        setModRMreg(1);
        setOperand(dst);
        setImm8(shift);
    }
    writeEnd();
}

void Assembler::ror8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::ror8(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xD3);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd2(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd2(Address dst, Reg src, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd4(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd4(Address dst, Reg src, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd8(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd8(Address dst, Reg src, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAC);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shrd8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shrd8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAD);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld2(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld2(Address dst, Reg src, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld4(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld4(Address dst, Reg src, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld8(Reg dst, Reg src, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld8(Address dst, Reg src, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA4);
    setModRMreg(src);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::shld8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::shld8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA5);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt2(Reg dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt2(Address dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt4(Reg dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt4(Address dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt8(Reg dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt8(Address dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(4);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bt8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bt8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FA3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts2(Reg dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts2(Address dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts4(Reg dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts4(Address dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts8(Reg dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts8(Address dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(5);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::bts8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bts8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FAB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr2(Reg dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr2(Address dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr4(Reg dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr4(Address dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr8(Reg dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr8(Address dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(6);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btr8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btr8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB3);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc2(Reg dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc2(Address dst, int imm8)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc4(Reg dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc4(Address dst, int imm8)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc8(Reg dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc8(Address dst, int imm8)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBA);
    setModRMreg(7);
    setOperand(dst);
    setImm8(imm8);
    writeEnd();
}

void Assembler::btc8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::btc8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBB);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bsf2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsf2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsf4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsf4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsf8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsf8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBC);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr2(Reg dst, Address src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr4(Reg dst, Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::bsr8(Reg dst, Address src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FBD);
    setModRMreg(dst);
    setOperand(src);
    writeEnd();
}

void Assembler::test1(Reg src1, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setOperand(src1);
    setImm8(imm8);
    writeEnd();
}

void Assembler::test1(Address src1, int imm8)
{
    writeBegin();
    setSize(1);
    setOpcode(0xF6);
    setOperand(src1);
    setImm8(imm8);
    writeEnd();
}

void Assembler::test1(Reg src1, Reg src2)
{
    writeBegin();
    setSize(1);
    setOpcode(0x84);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test1(Address src1, Reg src2)
{
    writeBegin();
    setSize(1);
    setOpcode(0x84);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test2(Reg src1, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setOperand(src1);
    setImm16(imm16);
    writeEnd();
}

void Assembler::test2(Address src1, int imm16)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xF7);
    setOperand(src1);
    setImm16(imm16);
    writeEnd();
}

void Assembler::test2(Reg src1, Reg src2)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test2(Address src1, Reg src2)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test4(Reg src1, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::test4(Address src1, int imm32)
{
    writeBegin();
    setSize(4);
    setOpcode(0xF7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::test4(Reg src1, Reg src2)
{
    writeBegin();
    setSize(4);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test4(Address src1, Reg src2)
{
    writeBegin();
    setSize(4);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test8(Reg src1, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::test8(Address src1, int imm32)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xF7);
    setOperand(src1);
    setImm32(imm32);
    writeEnd();
}

void Assembler::test8(Reg src1, Reg src2)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::test8(Address src1, Reg src2)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x85);
    setModRMreg(src2);
    setOperand(src1);
    writeEnd();
}

void Assembler::movs1()
{
    writeBegin();
    setSize(1);
    setOpcode(0xA4);
    writeEnd();
}

void Assembler::movs2()
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xA5);
    writeEnd();
}

void Assembler::movs4()
{
    writeBegin();
    setSize(4);
    setOpcode(0xA5);
    writeEnd();
}

void Assembler::movs8()
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xA5);
    writeEnd();
}

void Assembler::cmps1()
{
    writeBegin();
    setSize(1);
    setOpcode(0xA6);
    writeEnd();
}

void Assembler::cmps2()
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xA7);
    writeEnd();
}

void Assembler::cmps4()
{
    writeBegin();
    setSize(4);
    setOpcode(0xA7);
    writeEnd();
}

void Assembler::cmps8()
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xA7);
    writeEnd();
}

void Assembler::scas1()
{
    writeBegin();
    setSize(1);
    setOpcode(0xAE);
    writeEnd();
}

void Assembler::scas2()
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xAF);
    writeEnd();
}

void Assembler::scas4()
{
    writeBegin();
    setSize(4);
    setOpcode(0xAF);
    writeEnd();
}

void Assembler::scas8()
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xAF);
    writeEnd();
}

void Assembler::loads1()
{
    writeBegin();
    setSize(1);
    setOpcode(0xAC);
    writeEnd();
}

void Assembler::loads2()
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xAD);
    writeEnd();
}

void Assembler::loads4()
{
    writeBegin();
    setSize(4);
    setOpcode(0xAD);
    writeEnd();
}

void Assembler::loads8()
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xAD);
    writeEnd();
}

void Assembler::stos1()
{
    writeBegin();
    setSize(1);
    setOpcode(0xAA);
    writeEnd();
}

void Assembler::stos2()
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0xAB);
    writeEnd();
}

void Assembler::stos4()
{
    writeBegin();
    setSize(4);
    setOpcode(0xAB);
    writeEnd();
}

void Assembler::stos8()
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0xAB);
    writeEnd();
}

void Assembler::lea2(Reg dst, Address address)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x8D);
    setModRMreg(dst);
    setOperand(address);
    writeEnd();
}

void Assembler::lea4(Reg dst, Address address)
{
    writeBegin();
    setSize(4);
    setOpcode(0x8D);
    setModRMreg(dst);
    setOperand(address);
    writeEnd();
}

void Assembler::lea8(Reg dst, Address address)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x8D);
    setModRMreg(dst);
    setOperand(address);
    writeEnd();
}

void Assembler::xadd1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0FC0);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0FC0);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xadd8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FC1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::bswap4(Reg dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FC8);
    setOpfield(dst);
    writeEnd();
}

void Assembler::bswap8(Reg dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FC8);
    setOpfield(dst);
    writeEnd();
}

void Assembler::xchg1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x86);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x86);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::xchg8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x87);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg1(Reg dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0FB0);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg1(Address dst, Reg src)
{
    writeBegin();
    setSize(1);
    setOpcode(0x0FB0);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg2(Reg dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg2(Address dst, Reg src)
{
    writeBegin();
    setPrefix66();
    setSize(2);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg4(Reg dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg4(Address dst, Reg src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg8(Reg dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg8(Address dst, Reg src)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FB1);
    setModRMreg(src);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg8b(Address dst)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FC7);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::cmpxchg16(Address dst)
{
    writeBegin();
    setPrefixRexW();
    setSize(8);
    setOpcode(0x0FC7);
    setModRMreg(1);
    setOperand(dst);
    writeEnd();
}

void Assembler::stc()
{
    writeBegin();
    setOpcode(0xF9);
    writeEnd();
}

void Assembler::clc()
{
    writeBegin();
    setOpcode(0xF8);
    writeEnd();
}

void Assembler::cmc()
{
    writeBegin();
    setOpcode(0xF5);
    writeEnd();
}

void Assembler::std()
{
    writeBegin();
    setOpcode(0xFD);
    writeEnd();
}

void Assembler::cld()
{
    writeBegin();
    setOpcode(0xFC);
    writeEnd();
}

void Assembler::lahf()
{
    writeBegin();
    setOpcode(0x9F);
    writeEnd();
}

void Assembler::sahf()
{
    writeBegin();
    setOpcode(0x9E);
    writeEnd();
}

void Assembler::pushf()
{
    writeBegin();
    setOpcode(0x9C);
    writeEnd();
}

void Assembler::popf()
{
    writeBegin();
    setOpcode(0x9D);
    writeEnd();
}

void Assembler::nop()
{
    writeBegin();
    setOpcode(0x90);
    writeEnd();
}

void Assembler::ud2()
{
    writeBegin();
    setOpcode(0x91);
    writeEnd();
}

void Assembler::hlt()
{
    writeBegin();
    setOpcode(0x92);
    writeEnd();
}

void Assembler::interrupt(int imm8)
{
    writeBegin();
    setOpcode(0x93);
    setImm8(imm8);
    writeEnd();
}

void Assembler::clflush(Address src)
{
    writeBegin();
    setSize(4);
    setOpcode(0x0FAE);
    setModRMreg(7);
    setOperand(src);
    writeEnd();
}

void Assembler::lfence()
{
    writeBegin();
    setOpcode(0x0FAEE8);
    writeEnd();
}

void Assembler::sfence()
{
    writeBegin();
    setOpcode(0x0FAEF8);
    writeEnd();
}

void Assembler::mfence()
{
    writeBegin();
    setOpcode(0x0FAEF0);
    writeEnd();
}

void Assembler::cpuid()
{
    writeBegin();
    setOpcode(0x0FA2);
    writeEnd();
}


