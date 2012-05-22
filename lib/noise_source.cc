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

#include <gnuradio/extras/noise_source.h>
#include <gr_io_signature.h>
#include <gr_random.h>
#include <stdexcept>
#include <complex>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

using namespace gnuradio::extras;

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
class noise_source_impl : public noise_source{
public:
    noise_source_impl(const long seed):
        block(
            "noise source",
            gr_make_io_signature (0, 0, 0),
            gr_make_io_signature (1, 1, sizeof(type))
        ),
        _index(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0), _factor(9.0),
        _wave("GAUSSIAN"),
        _random(seed)
    {
        this->update_table();
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        _index += size_t(_random.ran1()*wave_table_size); //lookup into table is random each work()

        type *out = output_items[0].cast<type *>();
        for (size_t i = 0; i < output_items[0].size(); i++){
            out[i] = _table[_index % wave_table_size];
            _index++;
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

    void set_factor(const double &factor){
        _factor = factor;
        this->update_table();
    }

    double get_factor(void){
        return _factor;
    }

    void update_table(void){
        if (_wave == "UNIFORM"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::complex<double>(2*_random.ran1()-1, 2*_random.ran1()-1));
            }
        }
        else if (_wave == "GAUSSIAN"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::complex<double>(_random.gasdev(), _random.gasdev()));
            }
        }
        else if (_wave == "LAPLACIAN"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::complex<double>(_random.laplacian(), _random.laplacian()));
            }
        }
        else if (_wave == "IMPULSE"){
            const float factor = float(_factor);
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::complex<double>(_random.impulse(factor), _random.impulse(factor)));
            }
        }
        else throw std::invalid_argument("noise source got unknown wave type: " + _wave);
    }

    inline void set_elem(const size_t index, const std::complex<double> &val){
        gr_complex_double_to_num(_scalar * val + _offset, _table[index]);
    }

private:
    size_t _index;
    std::vector<type> _table;
    std::complex<double> _offset, _scalar;
    double _factor;
    std::string _wave;
    gr_random _random;
};

/***********************************************************************
 * factory function
 **********************************************************************/
noise_source::sptr noise_source::make_fc32(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<std::complex<float> >(seed));
}

noise_source::sptr noise_source::make_sc32(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<std::complex<int32_t> >(seed));
}

noise_source::sptr noise_source::make_sc16(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<std::complex<int16_t> >(seed));
}

noise_source::sptr noise_source::make_sc8(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<std::complex<int8_t> >(seed));
}

noise_source::sptr noise_source::make_f32(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<float>(seed));
}

noise_source::sptr noise_source::make_s32(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<int32_t>(seed));
}

noise_source::sptr noise_source::make_s16(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<int16_t>(seed));
}

noise_source::sptr noise_source::make_s8(const long seed){
    return gnuradio::get_initial_sptr(new noise_source_impl<int8_t>(seed));
}
