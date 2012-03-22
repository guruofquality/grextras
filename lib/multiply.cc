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
#include <boost/bind.hpp>

using namespace gnuradio::extras;

typedef boost::function<int(int, gr_vector_const_void_star, gr_vector_void_star)> volk_work_type;

/***********************************************************************
 * Multiplier implementation with complex float32 - calls volk
 **********************************************************************/
static int mult_fc32_work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    std::complex<float> *out = reinterpret_cast<std::complex<float> *>(output_items[0]);
    const std::complex<float> *in0 = reinterpret_cast<const std::complex<float> *>(input_items[0]);

    for (size_t n = 1; n < input_items.size(); n++){
        const std::complex<float> *in = reinterpret_cast<const std::complex<float> *>(input_items[n]);
        volk_32fc_x2_multiply_32fc_a(out, in0, in, noutput_items);
        in0 = out; //for next input, we do output += input
    }

    return noutput_items;
}

/***********************************************************************
 * Multiplier implementation with float32 - calls volk
 **********************************************************************/
static int mult_f32_work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    float *out = reinterpret_cast<float *>(output_items[0]);
    const float *in0 = reinterpret_cast<const float *>(input_items[0]);

    for (size_t n = 1; n < input_items.size(); n++){
        const float *in = reinterpret_cast<const float *>(input_items[n]);
        volk_32f_x2_multiply_32f_a(out, in0, in, noutput_items);
        in0 = out; //for next input, we do output += input
    }

    return noutput_items;
}

/***********************************************************************
 * Generic multiplyer implementation
 **********************************************************************/
template <typename type>
class multiply_generic : public multiply{
public:
    multiply_generic(const size_t vlen, const volk_work_type volk_work = volk_work_type()):
        gr_sync_block(
            "multiply generic",
            gr_make_io_signature (1, -1, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen),
        _volk_work(volk_work)
    {
        if (_volk_work){
            const int alignment_multiple = volk_get_alignment() / (sizeof(type)*vlen);
            set_output_multiple(std::max(1, alignment_multiple));
        }
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        if (_volk_work){
            _volk_work(noutput_items * _vlen, input_items, output_items);
            return noutput_items;
        }

        const size_t n_nums = noutput_items * _vlen;
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in0 = reinterpret_cast<const type *>(input_items[0]);

        for (size_t n = 1; n < input_items.size(); n++){
            const type *in = reinterpret_cast<const type *>(input_items[n]);
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in0[i] * in[i];
            }
            in0 = out; //for next input, we do output *= input
        }

        return noutput_items;
    }

private:
    const size_t _vlen;
    const volk_work_type _volk_work;
};

/***********************************************************************
 * factory function
 **********************************************************************/
multiply::sptr multiply::make_fc32_fc32(const size_t vlen){
    return sptr(new multiply_generic<float>(2*vlen, boost::bind(mult_fc32_work, _1, _2, _3)));
}

multiply::sptr multiply::make_sc32_sc32(const size_t vlen){
    return sptr(new multiply_generic<int32_t>(2*vlen));
}

multiply::sptr multiply::make_sc16_sc16(const size_t vlen){
    return sptr(new multiply_generic<int16_t>(2*vlen));
}

multiply::sptr multiply::make_sc8_sc8(const size_t vlen){
    return sptr(new multiply_generic<int8_t>(2*vlen));
}

multiply::sptr multiply::make_f32_f32(const size_t vlen){
    return sptr(new multiply_generic<float>(vlen, boost::bind(mult_f32_work, _1, _2, _3)));
}

multiply::sptr multiply::make_s32_s32(const size_t vlen){
    return sptr(new multiply_generic<int32_t>(vlen));
}

multiply::sptr multiply::make_s16_s16(const size_t vlen){
    return sptr(new multiply_generic<int16_t>(vlen));
}

multiply::sptr multiply::make_s8_s8(const size_t vlen){
    return sptr(new multiply_generic<int8_t>(vlen));
}
