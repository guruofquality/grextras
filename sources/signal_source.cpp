// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "sources_common.hpp"
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
struct SignalSource : gras::Block
{
    SignalSource(void):
        gras::Block("GrExtras SignalSource"),
        _index(0), _step(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0),
        _wave("CONST")
    {
        this->output_config(0).item_size = sizeof(type);
        this->update_table();
        this->register_call("set_waveform", &SignalSource::set_waveform);
        this->register_call("get_waveform", &SignalSource::get_waveform);
        this->register_call("set_offset", &SignalSource::set_offset);
        this->register_call("get_offset", &SignalSource::get_offset);
        this->register_call("set_amplitude", &SignalSource::set_amplitude);
        this->register_call("get_amplitude", &SignalSource::get_amplitude);
        this->register_call("set_frequency", &SignalSource::set_frequency);
        this->register_call("get_frequency", &SignalSource::get_frequency);
    }

    void work(const InputItems &, const OutputItems &outs)
    {
        type *out = outs[0].cast<type *>();
        for (size_t i = 0; i < outs[0].size(); i++)
        {
            out[i] = _table[_index % wave_table_size];
            _index += _step;
        }
        this->produce(0, outs[0].size());
    }

    void set_waveform(const std::string &wave)
    {
        _wave = wave;
        this->update_table();
    }

    std::string get_waveform(void)
    {
        return _wave;
    }

    void set_offset(const std::complex<double> &offset)
    {
        _offset = offset;
        this->update_table();
    }

    std::complex<double> get_offset(void)
    {
        return _offset;
    }

    void set_amplitude(const std::complex<double> &scalar)
    {
        _scalar = scalar;
        this->update_table();
    }

    std::complex<double> get_amplitude(void)
    {
        return _scalar;
    }

    void set_frequency(const double &freq)
    {
        _step = boost::math::iround(freq*_table.size());
    }

    double get_frequency(void)
    {
        return double(_step)/_table.size();
    }

    void update_table(void)
    {
        if (_wave == "CONST")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                this->set_elem(i, 1.0);
            }
        }
        else if (_wave == "COSINE")
        {
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::pow(2.718281828459, std::complex<double>(0, 6.28318530718*i/_table.size())));
            }
        }
        else if (_wave == "RAMP")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    2.0*i/(_table.size()-1) - 1.0,
                    2.0*q/(_table.size()-1) - 1.0
                ));
            }
        }
        else if (_wave == "SQUARE")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    (i < _table.size()/2)? 0.0 : 1.0,
                    (q < _table.size()/2)? 0.0 : 1.0
                ));
            }
        }
        else throw std::invalid_argument("sig source got unknown wave type: " + _wave);
    }

    inline void set_elem(const size_t index, const std::complex<double> &val)
    {
        complex128_to_num(_scalar * val + _offset, _table[index]);
    }

    size_t _index;
    size_t _step;
    std::vector<type> _table;
    std::complex<double> _offset, _scalar;
    std::string _wave;
};

/***********************************************************************
 * factory function
 **********************************************************************/
#define make_factory_function(suffix, type) \
static gras::Block *make_signal_source_ ## suffix(void) \
{ \
    return new SignalSource<type>(); \
} \
GRAS_REGISTER_FACTORY("/extras/signal_source_" #suffix, make_signal_source_ ## suffix)

make_factory_function(fc32, std::complex<float>)
make_factory_function(sc32, std::complex<boost::int32_t>)
make_factory_function(sc16, std::complex<boost::int16_t>)
make_factory_function(sc8, std::complex<boost::int8_t>)
make_factory_function(f32, float)
make_factory_function(s32, boost::int32_t)
make_factory_function(s16, boost::int16_t)
make_factory_function(s8, boost::int8_t)
