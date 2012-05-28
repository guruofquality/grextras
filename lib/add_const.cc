/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/extras/add_const.h>
#include <gr_io_signature.h>

using namespace gnuradio::extras;

//! This class is nothing more than a wrapper for the vlen=1 case of add_const_v

class add_const_derived : public add_const{
public:

    add_const_derived(const size_t size, add_const_v::sptr ub):
        gr_hier_block2(
            "add const wrapper",
            gr_make_io_signature (1, 1, size),
            gr_make_io_signature (1, 1, size)
        ),
        underlying_block(ub)
    {
        this->connect(this->self(), 0, ub, 0);
        this->connect(ub, 0, this->self(), 0);
    }

    void set_const(const std::complex<double> &val){
        underlying_block->set_const(
            std::vector<std::complex<double> >(1, val));
    }

    std::complex<double> get_const(void){
        return underlying_block->get_const().front();
    }

private:
    add_const_v::sptr underlying_block;
};

#define make_factory_function(suffix, type, op) \
    add_const::sptr add_const::make_ ## suffix( \
        const std::complex<double> &val \
    ){ \
        return gnuradio::get_initial_sptr(new add_const_derived( \
            sizeof(type), \
            add_const_v::make_ ## suffix( \
            std::vector<type>(1, type(val op))))); \
    }

make_factory_function(fc32_fc32, std::complex<float>, )
make_factory_function(sc32_sc32, std::complex<int32_t>, )
make_factory_function(sc16_sc16, std::complex<int16_t>, )
make_factory_function(sc8_sc8, std::complex<int8_t>, )
make_factory_function(f32_f32, float, .real())
make_factory_function(s32_s32, int32_t, .real())
make_factory_function(s16_s16, int16_t, .real())
make_factory_function(s8_s8, int8_t, .real())

