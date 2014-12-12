/**
 *  Copyright 2005-2009 NAKAMURA Minoru
 */

#include <assert.h>

#include "Address.hpp"

using namespace X64;

Address::Address(Reg base, int32_t disp)
    : _base(base), _index(Reg::invalid()), _scale(Scale::invalid), _disp(disp)
{
    assert(!_base.isInvalid());
    assert(!_base.isUpperHalf());
}

Address::Address(Reg base, Reg index, Scale scale, int32_t disp)
    : _base(base), _index(index), _scale(scale), _disp(disp)
{
    assert(!_base.isInvalid());
    assert(!_base.isUpperHalf());
    assert(!_index.isInvalid());
    assert(!_index.isUpperHalf());
    assert(!_scale.isInvalid());
    assert(_index != rsp);
}

