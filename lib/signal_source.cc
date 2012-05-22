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

#include <gnuradio/extras/signal_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace gnuradio::extras;

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
class signal_source_impl : public signal_source{
public:
    signal_source_impl(void):
        block(
            "signal source",
            gr_make_io_signature (0, 0, 0),
            gr_make_io_signature (1, 1, sizeof(type))
        ),
        _index(0), _step(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0),
        _wave("CONST")
    {
        this->update_table();
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        type *out = output_items[0].cast<type *>();
        for (size_t i = 0; i < output_items[0].size(); i++){
            out[i] = _table[_index % wave_table_size];
            _index += _step;
        }
        return output_items[0].size();
    }

    void set_waveform(const std::string &wave){
        _wave = wave;
        this->update_table();
    }

    std::string get_waveform(void){
        return _wave;
    }

    void set_offset(const std::complex<double> &offset){
        _offset = offset;
        this->update_table();
    }

    std::complex<double> get_offset(void){
        return _offset;
    }

    void set_amplitude(const std::complex<double> &scalar){
        _scalar = scalar;
        this->update_table();
    }

    std::complex<double> get_amplitude(void){
        return _scalar;
    }

    void set_frequency(const double freq){
        _step = boost::math::iround(freq*_table.size());
    }

    double get_frequency(void){
        return double(_step)/_table.size();
    }

    void update_table(void){
        if (_wave == "CONST"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, 1.0);
            }
        }
        else if (_wave == "COSINE"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::pow(M_E, std::complex<double>(0, M_PI*2*i/_table.size())));
            }
        }
        else if (_wave == "RAMP"){
            for (size_t i = 0; i < _table.size(); i++){
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    2.0*i/(_table.size()-1) - 1.0,
                    2.0*q/(_table.size()-1) - 1.0
                ));
            }
        }
        else if (_wave == "SQUARE"){
            for (size_t i = 0; i < _table.size(); i++){
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    (i < _table.size()/2)? 0.0 : 1.0,
                    (q < _table.size()/2)? 0.0 : 1.0
                ));
            }
        }
        else throw std::invalid_argument("sig source got unknown wave type: " + _wave);
    }

    inline void set_elem(const size_t index, const std::complex<double> &val){
        gr_complex_double_to_num(_scalar * val + _offset, _table[index]);
    }

private:
    size_t _index;
    size_t _step;
    std::vector<type> _table;
    std::complex<double> _offset, _scalar;
    std::string _wave;
};

/***********************************************************************
 * factory function
 **********************************************************************/
signal_source::sptr signal_source::make_fc32(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<std::complex<float> >());
}

signal_source::sptr signal_source::make_sc32(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<std::complex<int32_t> >());
}

signal_source::sptr signal_source::make_sc16(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<std::complex<int16_t> >());
}

signal_source::sptr signal_source::make_sc8(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<std::complex<int8_t> >());
}

signal_source::sptr signal_source::make_f32(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<float>());
}

signal_source::sptr signal_source::make_s32(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<int32_t>());
}

signal_source::sptr signal_source::make_s16(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<int16_t>());
}

signal_source::sptr signal_source::make_s8(void){
    return gnuradio::get_initial_sptr(new signal_source_impl<int8_t>());
}
