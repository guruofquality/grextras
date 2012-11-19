// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SIGNAL_SOURCE_HPP
#define INCLUDED_GREXTRAS_SIGNAL_SOURCE_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * Signal source block based on look up table for performance.
 * This block uses C++ templates to create multiple implementations.
 */
struct GREXTRAS_API SignalSource : virtual gras::Block
{
    typedef boost::shared_ptr<SignalSource> sptr;

    static sptr make_fc32(void);
    static sptr make_sc32(void);
    static sptr make_sc16(void);
    static sptr make_sc8(void);
    static sptr make_f32(void);
    static sptr make_s32(void);
    static sptr make_s16(void);
    static sptr make_s8(void);

    //! Set the waveform type (CONST, COSINE, RAMP, SQUARE)
    virtual void set_waveform(const std::string &) = 0;

    //! Get the current waveform setting
    virtual std::string get_waveform(void) = 0;

    //! Set the offset, this is a free addition operation
    virtual void set_offset(const std::complex<double> &) = 0;

    //! Get the current offset setting
    virtual std::complex<double> get_offset(void) = 0;

    //! Set the scalar, this is a free multiply scalar operation
    virtual void set_amplitude(const std::complex<double> &) = 0;

    //! Get the current amplitude setting
    virtual std::complex<double> get_amplitude(void) = 0;

    //! Set the frequency, this is a fractional number between -1 and 1
    virtual void set_frequency(const double) = 0;

    //! Get the actual frequency setting (-1 and 1)
    virtual double get_frequency(void) = 0;

    //! Convenience call to set frequency with sample rate
    void set_frequency(const double samp_rate, const double wave_freq)
    {
        return this->set_frequency(wave_freq/samp_rate);
    }

    //! Convenience call to get frequency with sample rate
    double get_frequency(const double samp_rate)
    {
        return samp_rate*this->get_frequency();
    }

};

}

#endif /*INCLUDED_GREXTRAS_SIGNAL_SOURCE_HPP*/
