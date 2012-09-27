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

#include <gnuradio/extras/multiply.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace gnuradio::extras;

/***********************************************************************
 * Templated multipler class
 **********************************************************************/
template <typename type>
class multiply_generic : public multiply{
public:
    multiply_generic(const size_t num_inputs, const size_t vlen):
        block(
            "multiply generic",
            gr_make_io_signature (num_inputs, num_inputs, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen)
    {
        const int alignment_multiple = get_work_multiple() / (sizeof(type)*vlen);
        set_output_multiple(std::max(1, alignment_multiple));
    }

    size_t get_work_multiple(void);

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    );

private:
    const size_t _vlen;
};

/***********************************************************************
 * Generic multiplier implementation
 **********************************************************************/
template <typename type>
size_t multiply_generic<type>::get_work_multiple(void)
{
    return 1;
}

template <typename type>
int multiply_generic<type>::work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    type *out = output_items[0].cast<type *>();
    const type *in0 = input_items[0].cast<const type *>();

    for (size_t n = 1; n < input_items.size(); n++){
        const type *in = input_items[n].cast<const type *>();
        for (size_t i = 0; i < n_nums; i++){
            out[i] = in0[i] * in[i];
        }
        in0 = out; //for next input, we do output += input
    }

    return output_items[0].size();
}

/***********************************************************************
 * Multiplier implementation with complex complex float32 - calls volk
 **********************************************************************/
template <>
size_t multiply_generic<std::complex<float> >::get_work_multiple(void)
{
    return volk_get_alignment();
}

template <>
int multiply_generic<std::complex<float> >::work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    std::complex<float> *out = output_items[0].cast<std::complex<float> *>();
    const std::complex<float> *in0 = input_items[0].cast<const std::complex<float> *>();

    for (size_t n = 1; n < input_items.size(); n++){
        const std::complex<float> *in = input_items[n].cast<const std::complex<float> *>();
        volk_32fc_x2_multiply_32fc_a(out, in0, in, n_nums);
        in0 = out; //for next input, we do output += input
    }
    return output_items[0].size();
}

/***********************************************************************
 * Multiplier implementation with float32 - calls volk
 **********************************************************************/
template <>
size_t multiply_generic<float>::get_work_multiple(void)
{
    return volk_get_alignment();
}

template <>
int multiply_generic<float>::work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    float *out = output_items[0].cast<float *>();
    const float *in0 = input_items[0].cast<const float *>();

    for (size_t n = 1; n < input_items.size(); n++){
        const float *in = input_items[n].cast<const float *>();
        volk_32f_x2_multiply_32f_a(out, in0, in, n_nums);
        in0 = out; //for next input, we do output += input
    }
    return output_items[0].size();
}

/***********************************************************************
 * factory function
 **********************************************************************/
multiply::sptr multiply::make_fc32_fc32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<std::complex<float> >(num_inputs, vlen));
}

multiply::sptr multiply::make_sc32_sc32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int32_t>(num_inputs, 2*vlen));
}

multiply::sptr multiply::make_sc16_sc16(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int16_t>(num_inputs, 2*vlen));
}

multiply::sptr multiply::make_sc8_sc8(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int8_t>(num_inputs, 2*vlen));
}

multiply::sptr multiply::make_f32_f32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<float>(num_inputs, vlen));
}

multiply::sptr multiply::make_s32_s32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int32_t>(num_inputs, vlen));
}

multiply::sptr multiply::make_s16_s16(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int16_t>(num_inputs, vlen));
}

multiply::sptr multiply::make_s8_s8(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new multiply_generic<int8_t>(num_inputs, vlen));
}
