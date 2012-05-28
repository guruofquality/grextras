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

using namespace gnuradio::extras;

/***********************************************************************
 * FIR filter FC32 implementation
 **********************************************************************/
struct decim_fir_fc32_work{
    int operator()(
        const gnuradio::block::InputItems &input_items,
        const gnuradio::block::OutputItems &output_items,
        const void *taps_ptr,
        const size_t num_taps,
        const size_t decim
    ){
        typedef std::complex<float> type;
        type *out = output_items[0].cast<type *>();
        const type *in = input_items[0].cast<const type *>();
        const type *taps = reinterpret_cast<const type *>(taps_ptr);

        for (size_t i = 0; i < output_items[0].size(); i++){
            volk_32fc_x2_dot_prod_32fc_u(out+i, in, taps, num_taps);
            in += decim;
        }

        return output_items[0].size();
    }
};

/***********************************************************************
 * FIR filter F32 implementation
 **********************************************************************/
struct decim_fir_f32_work{
    int operator()(
        const gnuradio::block::InputItems &input_items,
        const gnuradio::block::OutputItems &output_items,
        const void *taps_ptr,
        const size_t num_taps,
        const size_t decim
    ){
        typedef float type;
        type *out = output_items[0].cast<type *>();
        const type *in = input_items[0].cast<const type *>();
        const type *taps = reinterpret_cast<const type *>(taps_ptr);

        for (size_t i = 0; i < output_items[0].size(); i++){
            volk_32f_x2_dot_prod_32f_u(out+i, in, taps, num_taps);
            in += decim;
        }

        return output_items[0].size();
    }
};

/***********************************************************************
 * FIR filter generic implementation
 **********************************************************************/
template <typename intype, typename tapstype, typename outtype, typename WorkType>
class generic_decim_fir : public decim_fir{
public:

    generic_decim_fir(const size_t decim):
        block(
            "FIR filter",
            gr_make_io_signature (1, 1, sizeof(intype)),
            gr_make_io_signature (1, 1, sizeof(outtype))
        )
    {
        this->set_relative_rate(1.0/decim);
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        gruel::scoped_lock lock(_taps_mutex);
        _volk_work(
            input_items, output_items,
            &_converted_taps.front(),
            this->history(),
            size_t(1.0/this->relative_rate())
        );
        return output_items[0].size();
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
    WorkType _volk_work;
};

/***********************************************************************
 * FIR filter factory function
 **********************************************************************/
decim_fir::sptr decim_fir::make_fc32_fc32_fc32(
    const taps_type &taps, const size_t decim
){
    decim_fir::sptr b = gnuradio::get_initial_sptr(new generic_decim_fir
        <std::complex<float>, std::complex<float>, std::complex<float>, decim_fir_fc32_work >(decim));
    b->set_taps(taps);
    return b;
}

decim_fir::sptr decim_fir::make_f32_f32_f32(
    const taps_type &taps, const size_t decim
){
    decim_fir::sptr b = gnuradio::get_initial_sptr(new generic_decim_fir
        <float, float, float, decim_fir_f32_work>(decim));
    b->set_taps(taps);
    return b;
}
