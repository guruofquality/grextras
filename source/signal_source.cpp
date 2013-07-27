// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/signal_source.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

using namespace grextras;

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
struct SignalSourceImpl : public SignalSource
{
    SignalSourceImpl(void):
        gras::Block("GrExtras SignalSource"),
        _index(0), _step(0),
        _table(wave_table_size),
        _offset(0.0), _scalar(1.0),
        _wave("CONST")
    {
        this->output_config(0).item_size = sizeof(type);
        this->update_table();
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

    void set_frequency(const double freq)
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
SignalSource::sptr SignalSource::make_fc32(void)
{
    return sptr(new SignalSourceImpl<std::complex<float> >());
}

SignalSource::sptr SignalSource::make_sc32(void)
{
    return sptr(new SignalSourceImpl<std::complex<boost::int32_t> >());
}

SignalSource::sptr SignalSource::make_sc16(void)
{
    return sptr(new SignalSourceImpl<std::complex<boost::int16_t> >());
}

SignalSource::sptr SignalSource::make_sc8(void)
{
    return sptr(new SignalSourceImpl<std::complex<boost::int8_t> >());
}

SignalSource::sptr SignalSource::make_f32(void)
{
    return sptr(new SignalSourceImpl<float>());
}

SignalSource::sptr SignalSource::make_s32(void)
{
    return sptr(new SignalSourceImpl<boost::int32_t>());
}

SignalSource::sptr SignalSource::make_s16(void)
{
    return sptr(new SignalSourceImpl<boost::int16_t>());
}

SignalSource::sptr SignalSource::make_s8(void)
{
    return sptr(new SignalSourceImpl<boost::int8_t>());
}
