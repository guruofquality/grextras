// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/noise_source.hpp>
#include <boost/make_shared.hpp>
#ifdef HAVE_GNURADIO_CORE
#include <gr_random.h>
#else
typedef long gr_random;
#endif
#include <stdexcept>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

using namespace grextras;

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
class NoiseSourceImpl : public NoiseSource{
public:
    NoiseSourceImpl(const long seed):
        gras::Block("GrExtras Noise Source"),
        _index(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0), _factor(9.0),
        _wave("GAUSSIAN"),
        _random(seed)
    {
        this->output_config(0).item_size = sizeof(type);
        this->update_table();
    }

    void work(const InputItems &, const OutputItems &outs)
    {
        #ifdef HAVE_GNURADIO_CORE
        _index += size_t(_random.ran1()*wave_table_size); //lookup into table is random each work()
        #endif

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
        #ifndef HAVE_GNURADIO_CORE
        throw std::runtime_error("noise source needs gr_random, but was build without GNU Radio core support!");
        #else
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
        #endif
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
NoiseSource::sptr NoiseSource::make_fc32(const long seed)
{
    return sptr(new NoiseSourceImpl<std::complex<float> >(seed));
}

NoiseSource::sptr NoiseSource::make_sc32(const long seed)
{
    return sptr(new NoiseSourceImpl<std::complex<boost::int32_t> >(seed));
}

NoiseSource::sptr NoiseSource::make_sc16(const long seed)
{
    return sptr(new NoiseSourceImpl<std::complex<boost::int16_t> >(seed));
}

NoiseSource::sptr NoiseSource::make_sc8(const long seed)
{
    return sptr(new NoiseSourceImpl<std::complex<boost::int8_t> >(seed));
}

NoiseSource::sptr NoiseSource::make_f32(const long seed)
{
    return sptr(new NoiseSourceImpl<float>(seed));
}

NoiseSource::sptr NoiseSource::make_s32(const long seed)
{
    return sptr(new NoiseSourceImpl<boost::int32_t>(seed));
}

NoiseSource::sptr NoiseSource::make_s16(const long seed)
{
    return sptr(new NoiseSourceImpl<boost::int16_t>(seed));
}

NoiseSource::sptr NoiseSource::make_s8(const long seed)
{
    return sptr(new NoiseSourceImpl<boost::int8_t>(seed));
}
