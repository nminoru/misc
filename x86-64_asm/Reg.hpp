/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#if !defined(__X64__REG_HPP__)
#define __X64__REG_HPP__

#include <stdint.h>

namespace X64
{

/**
 *  Intel64 汎用レジスタ
 */
class Reg {

private:
    int   _id;
    bool  _isUH; // Upper Half

public:
    Reg() :
        _id(-1), _isUH(false) {}

    Reg(int id, bool isUH) :
        _id(id), _isUH(isUH) {}

    Reg(const Reg& other) :
        _id(other._id), _isUH(other._isUH) {}

    bool operator==(const Reg& other) const {
        return (_id == other._id) && (_isUH == other._isUH);
    }

    bool operator!=(const Reg& other) const {
        return (_id != other._id) || (_isUH != other._isUH);
    }

    // Id getId() const { return _id; }

    bool isInvalid() const {
        return _id == -1;
    }

    bool isUpperHalf() const {
        return _isUH;
    }

    bool needsRex() const {
        return (8 <= _id) && (_id <= 15);
    }

    bool needsRexIfByte() const {
        return (4 <= _id) && (_id <= 15);
    }

    int getEncoding() const {
        if (_isUH) {
            return _id + 4;
        } else {
            return _id;
        }
    }

    static Reg invalid();
};



extern Reg r0;
extern Reg r1;
extern Reg r2;
extern Reg r3;
extern Reg r4;
extern Reg r5;
extern Reg r6;
extern Reg r7;
extern Reg r8;
extern Reg r9;
extern Reg r10;
extern Reg r11;
extern Reg r12;
extern Reg r13;
extern Reg r14;
extern Reg r15;
extern Reg rax;
extern Reg rcx;
extern Reg rdx;
extern Reg rbx;
extern Reg rsp;
extern Reg rbp;
extern Reg rsi;
extern Reg rdi;
extern Reg ah;
extern Reg ch;
extern Reg dh;
extern Reg bh;
extern Reg rarg0; // rdi
extern Reg rarg1; // rsi
extern Reg rarg2; // rdx
extern Reg rarg3; // rcx
extern Reg rarg4; // r8
extern Reg rarg5; // r9



/**
 *  128-bit SSE レジスタ
 */
class XmmReg {

public:
    enum {
        invalid  = -1,

        xmm0     =  0,
        xmm1     =  1,
        xmm2     =  2,
        xmm3     =  3,
        xmm4     =  4,
        xmm5     =  5,
        xmm6     =  6,
        xmm7     =  7,
        xmm8     =  8,
        xmm9     =  9,
        xmm10    = 10,
        xmm11    = 11,
        xmm12    = 12,
        xmm13    = 13,
        xmm14    = 14,
        xmm15    = 15,

        farg0    = xmm0,
        farg1    = xmm1,
        farg2    = xmm2,
        farg3    = xmm3,
        farg4    = xmm4,
        farg5    = xmm5,
        farg6    = xmm6,
        farg7    = xmm7,
    };

private:
    int   _id;

public:
    XmmReg() :
        _id(-1) {}

    XmmReg(const XmmReg& other) :
        _id(other._id) {}

    bool operator==(const XmmReg& other) const {
        return _id == other._id;
    }

    bool operator!=(const XmmReg& other) const {
        return _id != other._id;
    }

    bool isInvalid() const {
        return _id == -1;
    }

public:
    const char* name() const;
};



}

#endif /* __X64__REG_HPP__ */
