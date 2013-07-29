// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "sources_common.hpp"
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include "noise_source_random.hpp"
#include <stdexcept>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
struct NoiseSource : gras::Block
{
    NoiseSource(const long seed):
        gras::Block("GrExtras Noise Source"),
        _index(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0), _factor(9.0),
        _wave("GAUSSIAN"),
        _random(seed)
    {
        this->output_config(0).item_size = sizeof(type);
        this->update_table();
        this->register_call("set_waveform", &NoiseSource::set_waveform);
        this->register_call("get_waveform", &NoiseSource::get_waveform);
        this->register_call("set_offset", &NoiseSource::set_offset);
        this->register_call("get_offset", &NoiseSource::get_offset);
        this->register_call("set_amplitude", &NoiseSource::set_amplitude);
        this->register_call("get_amplitude", &NoiseSource::get_amplitude);
        this->register_call("set_factor", &NoiseSource::set_factor);
        this->register_call("get_factor", &NoiseSource::get_factor);
    }

    void work(const InputItems &, const OutputItems &outs)
    {
        _index += size_t(_random.ran1()*wave_table_size); //lookup into table is random each work()

        type *out = outs[0].cast<type *>();
        for (size_t i = 0; i < outs[0].size(); i++)
        {
            out[i] = _table[_index % wave_table_size];
            _index++;
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

    void set_factor(const double &factor)
    {
        _factor = factor;
        this->update_table();
    }

    double get_factor(void)
    {
        return _factor;
    }

    void update_table(void)
    {
        if (_wave == "UNIFORM")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                this->set_elem(i, std::complex<double>(2*_random.ran1()-1, 2*_random.ran1()-1));
            }
        }
        else if (_wave == "GAUSSIAN")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                this->set_elem(i, std::complex<double>(_random.gasdev(), _random.gasdev()));
            }
        }
        else if (_wave == "LAPLACIAN")
        {
            for (size_t i = 0; i < _table.size(); i++)
            {
                this->set_elem(i, std::complex<double>(_random.laplacian(), _random.laplacian()));
            }
        }
        else if (_wave == "IMPULSE")
        {
            const float factor = float(_factor);
            for (size_t i = 0; i < _table.size(); i++)
            {
                this->set_elem(i, std::complex<double>(_random.impulse(factor), _random.impulse(factor)));
            }
        }
        else throw std::invalid_argument("noise source got unknown wave type: " + _wave);
    }

    inline void set_elem(const size_t index, const std::complex<double> &val)
    {
        complex128_to_num(_scalar * val + _offset, _table[index]);
    }

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
#define make_factory_function(suffix, type) \
static gras::Block *make_noise_source_ ## suffix(const long &seed) \
{ \
    return new NoiseSource<type>(seed); \
} \
GRAS_REGISTER_FACTORY("/extras/noise_source_" #suffix, make_noise_source_ ## suffix)

make_factory_function(fc32, std::complex<float>)
make_factory_function(sc32, std::complex<boost::int32_t>)
make_factory_function(sc16, std::complex<boost::int16_t>)
make_factory_function(sc8, std::complex<boost::int8_t>)
make_factory_function(f32, float)
make_factory_function(s32, boost::int32_t)
make_factory_function(s16, boost::int16_t)
make_factory_function(s8, boost::int8_t)
