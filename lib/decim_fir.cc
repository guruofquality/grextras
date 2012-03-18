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

#include <gnuradio/extras/decim_fir.h>
#include <gr_io_signature.h>
#include <gruel/thread.h>
#include <algorithm>
#include <stdexcept>
#include <volk/volk.h>
#include <iostream>
#include <boost/bind.hpp>

using namespace gnuradio::extras;

typedef boost::function<int(int, gr_vector_const_void_star, gr_vector_void_star, const void *, const size_t, const size_t)> volk_work_type;

/***********************************************************************
 * FIR filter FC32 implementation
 **********************************************************************/
static int decim_fir_fc32_work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items,
    const void *taps_ptr,
    const size_t num_taps,
    const size_t decim
){
    typedef std::complex<float> type;
    type *out = reinterpret_cast<type *>(output_items[0]);
    const type *in = reinterpret_cast<const type *>(input_items[0]);
    const type *taps = reinterpret_cast<const type *>(taps_ptr);

    for (size_t i = 0; i < size_t(noutput_items); i++){
        volk_32fc_x2_dot_prod_32fc_u(out+i, in, taps, num_taps);
        in += decim;
    }

    return noutput_items;
}

/***********************************************************************
 * FIR filter F32 implementation
 **********************************************************************/
static int decim_fir_f32_work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items,
    const void *taps_ptr,
    const size_t num_taps,
    const size_t decim
){
    typedef float type;
    type *out = reinterpret_cast<type *>(output_items[0]);
    const type *in = reinterpret_cast<const type *>(input_items[0]);
    const type *taps = reinterpret_cast<const type *>(taps_ptr);

    for (size_t i = 0; i < size_t(noutput_items); i++){
        volk_32f_x2_dot_prod_32f_u(out+i, in, taps, num_taps);
        in += decim;
    }

    return noutput_items;
}

/***********************************************************************
 * FIR filter generic implementation
 **********************************************************************/
template <typename intype, typename tapstype, typename outtype>
class generic_decim_fir : public decim_fir{
public:

    generic_decim_fir(const size_t decim, const volk_work_type volk_work):
        gr_sync_decimator(
            "FIR filter",
            gr_make_io_signature (1, 1, sizeof(intype)),
            gr_make_io_signature (1, 1, sizeof(outtype)),
            decim
        ),
        _volk_work(volk_work)
    {
        //NOP
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock lock(_taps_mutex);
        _volk_work(
            noutput_items, input_items, output_items,
            &_converted_taps.front(), this->history(),
            this->decimation()
        );
        return noutput_items;
    }

    void set_taps(const taps_type &taps){
        gruel::scoped_lock lock(_taps_mutex);

        //copy the new taps in and update history
        _converted_taps.resize(taps.size());
        _original_taps.resize(taps.size());
        for (size_t i = 0; i < taps.size(); i++){
            _converted_taps[i] = tapstype(taps[i].real());
            _original_taps[i] = taps[i];
        }
        std::reverse(_converted_taps.begin(), _converted_taps.end());
        this->set_history(_converted_taps.size());
    }

    taps_type get_taps(void){
        gruel::scoped_lock lock(_taps_mutex);
        return _original_taps;
    }

private:
    gruel::mutex _taps_mutex;
    std::vector<tapstype> _converted_taps;
    taps_type _original_taps;
    const volk_work_type _volk_work;
};

/***********************************************************************
 * FIR filter factory function
 **********************************************************************/
decim_fir::sptr decim_fir::make_fc32_fc32_fc32(
    const taps_type &taps, const size_t decim
){
    decim_fir::sptr b(new generic_decim_fir<std::complex<float>, std::complex<float>, std::complex<float> >(
        decim, boost::bind(&decim_fir_fc32_work, _1, _2, _3, _4, _5, _6)));
    b->set_taps(taps);
    return b;
}

decim_fir::sptr decim_fir::make_f32_f32_f32(
    const taps_type &taps, const size_t decim
){
    decim_fir::sptr b(new generic_decim_fir<float, float, float>(
        decim, boost::bind(&decim_fir_f32_work, _1, _2, _3, _4, _5, _6)));
    b->set_taps(taps);
    return b;
}
