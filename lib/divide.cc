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

#include <gnuradio/extras/divide.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace gnuradio::extras;

/***********************************************************************
 * Generic divideer implementation
 **********************************************************************/
template <typename type>
class divide_generic : public divide{
public:
    divide_generic(const size_t num_inputs, const size_t vlen):
        block(
            "divide generic",
            gr_make_io_signature (num_inputs, num_inputs, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen)
    {
        //NOP
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        const size_t noutput_items = output_items[0].size();

        if(input_items.size() == 1) {
            const size_t n_nums = noutput_items * _vlen;
            type *out = output_items[0].cast<type *>();
            const type *in = input_items[0].cast<const type *>();

            for (size_t i = 0; i < n_nums; i++){
                out[i] = static_cast<type>(1) / in[i];
            }

            return noutput_items;
        }
        else {
            const size_t n_nums = noutput_items * _vlen;
            type *out = output_items[0].cast<type *>();
            const type *in0 = input_items[0].cast<const type *>();

            for (size_t n = 1; n < input_items.size(); n++){
                const type *in = input_items[n].cast<const type *>();
                for (size_t i = 0; i < n_nums; i++){
                    out[i] = in0[i] / in[i];
                }
                in0 = out; //for next input, we do output /= input
            }

            return noutput_items;
        }
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * factory function
 **********************************************************************/
divide::sptr divide::make_fc32_fc32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<float>(num_inputs, 2*vlen));
}

divide::sptr divide::make_sc32_sc32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int32_t>(num_inputs, 2*vlen));
}

divide::sptr divide::make_sc16_sc16(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int16_t>(num_inputs, 2*vlen));
}

divide::sptr divide::make_sc8_sc8(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int8_t>(num_inputs, 2*vlen));
}

divide::sptr divide::make_f32_f32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<float>(num_inputs, vlen));
}

divide::sptr divide::make_s32_s32(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int32_t>(num_inputs, vlen));
}

divide::sptr divide::make_s16_s16(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int16_t>(num_inputs, vlen));
}

divide::sptr divide::make_s8_s8(const size_t num_inputs, const size_t vlen){
    return gnuradio::get_initial_sptr(new divide_generic<int8_t>(num_inputs, vlen));
}
