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

#include <gnuradio/extras/multiply_const.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace gnuradio::extras;

/***********************************************************************
 * Generic multiply const implementation
 **********************************************************************/
template <typename type>
class multiply_const_generic : public multiply_const_v{
public:
    multiply_const_generic(const std::vector<type> &vec):
        block(
            "multiply const generic",
            gr_make_io_signature (1, 1, sizeof(type)*vec.size()),
            gr_make_io_signature (1, 1, sizeof(type)*vec.size())
        )
    {
        _val.resize(vec.size());
        this->set_const(vec);
        const int alignment_multiple = get_work_multiple() / (sizeof(type)*_val.size());
        set_output_multiple(std::max(1, alignment_multiple));
    }

    size_t get_work_multiple(void);

    int work1(
        const InputItems &input_items,
        const OutputItems &output_items
    );

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        if (_val.size() == 1) return work1(input_items, output_items);

        const size_t noutput_items = output_items[0].size();
        const size_t n_nums = noutput_items * _val.size();
        type *out = output_items[0].cast<type *>();
        const type *in = input_items[0].cast<const type *>();

        for (size_t i = 0; i < n_nums; i++){
            out[i] = in[i] * _val[i%_val.size()];
        }

        return noutput_items;
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
 * Generic vlen == 1 multiplier implementation
 **********************************************************************/
template <typename type>
size_t multiply_const_generic<type>::get_work_multiple(void)
{
    return 1;
}

template <typename type>
int multiply_const_generic<type>::work1(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t noutput_items = output_items[0].size();
    type *out = output_items[0].cast<type *>();
    const type *in = input_items[0].cast<const type *>();

    for (size_t i = 0; i < noutput_items; i++){
        out[i] = in[i] * _val[0];
    }

    return noutput_items;
}

/***********************************************************************
 * FC32 vlen == 1 multiplier implementation
 **********************************************************************/
template <>
size_t multiply_const_generic<std::complex<float> >::get_work_multiple(void)
{
    return volk_get_alignment();
}

template <>
int multiply_const_generic<std::complex<float> >::work1(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t num_items = output_items[0].size();
    typedef std::complex<float> type;
    const type scalar = reinterpret_cast<const type *>(&_val[0])[0];
    type *out = output_items[0].cast<type *>();
    const type *in = input_items[0].cast<const type *>();
    volk_32fc_s32fc_multiply_32fc_a(out, in, scalar, num_items);
    return num_items;
}

/***********************************************************************
 * F32 vlen == 1 multiplier implementation
 **********************************************************************/
template <>
size_t multiply_const_generic<float>::get_work_multiple(void)
{
    return volk_get_alignment();
}

template <>
int multiply_const_generic<float>::work1(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t num_items = output_items[0].size();
    typedef float type;
    const type scalar = reinterpret_cast<const type *>(&_val[0])[0];
    type *out = output_items[0].cast<type *>();
    const type *in = input_items[0].cast<const type *>();
    volk_32f_s32f_multiply_32f_a(out, in, scalar, num_items);
    return num_items;
}

/***********************************************************************
 * factory function
 **********************************************************************/
#define make_factory_function(suffix, type) \
    multiply_const_v::sptr multiply_const_v::make_ ## suffix(const std::vector<type > &vec){ \
    return gnuradio::get_initial_sptr(new multiply_const_generic<type>(vec)); \
}

make_factory_function(fc32_fc32, std::complex<float>)
make_factory_function(sc32_sc32, std::complex<int32_t>)
make_factory_function(sc16_sc16, std::complex<int16_t>)
make_factory_function(sc8_sc8, std::complex<int8_t>)
make_factory_function(f32_f32, float)
make_factory_function(s32_s32, int32_t)
make_factory_function(s16_s16, int16_t)
make_factory_function(s8_s8, int8_t)
