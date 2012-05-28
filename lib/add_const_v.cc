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
#include <stdexcept>
#include <complex>

using namespace gnuradio::extras;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
class add_const_generic : public add_const_v{
public:
    add_const_generic(const std::vector<type> &vec):
        block(
            "add const generic",
            gr_make_io_signature (1, 1, sizeof(type)*vec.size()),
            gr_make_io_signature (1, 1, sizeof(type)*vec.size())
        )
    {
        _val.resize(vec.size());
        this->set_const(vec);
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        const size_t n_nums = output_items.size() * _val.size();
        type *out = output_items[0].cast<type *>();
        const type *in = input_items[0].cast<const type *>();

        //simple vec len 1 for the fast
        if (_val.size() == 1){
            const type val = _val[0];
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in[i] + val;
            }
        }

        //general case for any vlen
        else{
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in[i] + _val[i%_val.size()];
            }
        }
        return output_items.size();
    }

    void _set_const(const std::vector<std::complex<double> > &val){
        _original_val.assign(val.begin(), val.end());

        if (val.size() != _val.size()){
            throw std::invalid_argument("set_const called with the wrong length");
        }
        for (size_t i = 0; i < val.size(); i++){
            gr_complex_double_to_num(val[i], _val[i]);
        }
    }

    std::vector<std::complex<double> > get_const(void){
        return _original_val;
    }

private:
    std::vector<std::complex<double> > _original_val;
    std::vector<type> _val;
};

/***********************************************************************
 * factory function
 **********************************************************************/
#define make_factory_function(suffix, type) \
    add_const_v::sptr add_const_v::make_ ## suffix(const std::vector<type > &vec){ \
    return gnuradio::get_initial_sptr(new add_const_generic<type >(vec)); \
}

make_factory_function(fc32_fc32, std::complex<float>)
make_factory_function(sc32_sc32, std::complex<int32_t>)
make_factory_function(sc16_sc16, std::complex<int16_t>)
make_factory_function(sc8_sc8, std::complex<int8_t>)
make_factory_function(f32_f32, float)
make_factory_function(s32_s32, int32_t)
make_factory_function(s16_s16, int16_t)
make_factory_function(s8_s8, int8_t)
