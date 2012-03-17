/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <gnuradio/blocks/divide.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace gnuradio::blocks;

/***********************************************************************
 * Generic divideer implementation
 **********************************************************************/
template <typename type>
class divide_generic : public divide{
public:
    divide_generic(const size_t vlen):
        gr_sync_block(
            "divide generic",
            gr_make_io_signature (1, -1, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen)
    {
        //NOP
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        if(input_items.size() == 1) {
            const size_t n_nums = noutput_items * _vlen;
            type *out = reinterpret_cast<type *>(output_items[0]);
            const type *in = reinterpret_cast<const type *>(input_items[0]);

            for (size_t i = 0; i < n_nums; i++){
                out[i] = static_cast<type>(1) / in[i];
            }

            return noutput_items;
        }
        else {
            const size_t n_nums = noutput_items * _vlen;
            type *out = reinterpret_cast<type *>(output_items[0]);
            const type *in0 = reinterpret_cast<const type *>(input_items[0]);

            for (size_t n = 1; n < input_items.size(); n++){
                const type *in = reinterpret_cast<const type *>(input_items[n]);
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
divide::sptr divide::make_fc32_fc32(const size_t vlen){
    return sptr(new divide_generic<float>(2*vlen));
}

divide::sptr divide::make_sc32_sc32(const size_t vlen){
    return sptr(new divide_generic<uint32_t>(2*vlen));
}

divide::sptr divide::make_sc16_sc16(const size_t vlen){
    return sptr(new divide_generic<uint16_t>(2*vlen));
}

divide::sptr divide::make_sc8_sc8(const size_t vlen){
    return sptr(new divide_generic<uint8_t>(2*vlen));
}

divide::sptr divide::make_f32_f32(const size_t vlen){
    return sptr(new divide_generic<float>(vlen));
}

divide::sptr divide::make_s32_s32(const size_t vlen){
    return sptr(new divide_generic<uint32_t>(vlen));
}

divide::sptr divide::make_s16_s16(const size_t vlen){
    return sptr(new divide_generic<uint16_t>(vlen));
}

divide::sptr divide::make_s8_s8(const size_t vlen){
    return sptr(new divide_generic<uint8_t>(vlen));
}
