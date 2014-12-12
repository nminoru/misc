/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#if !defined(__X64__ADDRESS_HPP__)
#define __X64__ADDRESS_HPP__

#include <stdio.h>
#include <stdint.h>

#include "Reg.hpp"



namespace X64
{

class Label;


/**
 *  RelocationType 
 */
enum RelocationType {
    RT_CODE_CACHE_HEADER,
    RT_COMPILE_UNIT_HEADER,
    RT_COMPILE_UNIT_GS_MEMORY,
    RT_COMPILE_UNIT_CONSTANT_POOL,
};



/**
 *  SIB のスケール
 */
class Scale {

public:
    enum Times {
        invalid = 0,

        times1 = 1,
        times2 = 2,
        times4 = 3,
        times8 = 4,
    };

private:
    Times _times;
  
public:
    Scale() : _times(invalid) {}
    Scale(Times times) : _times(times) {}
    Scale(const Scale& other) : _times(other._times) {}

    Times getTimes() const { return _times; }
    int   getEncoding() const { return _times - times1; }

    bool isInvalid() const {
        return _times == invalid;
    }
};



/**
 *  Assembler 中でメモリアドレス(SIB + 即値)を示すクラス
 *
 *  TODO: rip を中心とした relocation にも対応せよ
 */
class Address {

private:
    Reg      _base;
    Reg      _index;
    Scale    _scale;
    int32_t  _disp;

public:
    Address()
        : _base(Reg::invalid()), _index(Reg::invalid()), _scale(Scale::invalid), _disp(0)
        {}

    Address(const Address& other)
        : _base(other._base), _index(other._index), _scale(other._scale), _disp(other._disp)
        {}

    Address(Reg base, int32_t disp = 0);
    Address(Reg base, Reg index, Scale scale, int32_t disp = 0);
    
    // RIP 相対でアドレス指定をする
    Address(Label& label); // unimplemented
    
    // リロケーション指定
    Address(RelocationType rtype, int offset); // unimplemented

    const Reg& getBase() const { return _base; }
    const Reg& getIndex() const { return _index; }
    const Scale& getScale() const { return _scale; }
    int32_t getDisp() const { return _disp; }
    bool  isSimm8() const { return (-128 <= _disp) && (_disp <= 127); } 
};


};

#endif /* __X64__REG_HPP__ */
