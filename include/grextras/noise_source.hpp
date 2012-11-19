// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_NOISE_SOURCE_HPP
#define INCLUDED_GREXTRAS_NOISE_SOURCE_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * Noise source block based on look up table for performance.
 * This block uses C++ templates to create multiple implementations.
 */
struct GREXTRAS_API NoiseSource : virtual gras::Block
{
    typedef boost::shared_ptr<NoiseSource> sptr;

    static sptr make_fc32(const long seed = -42);
    static sptr make_sc32(const long seed = -42);
    static sptr make_sc16(const long seed = -42);
    static sptr make_sc8(const long seed = -42);
    static sptr make_f32(const long seed = -42);
    static sptr make_s32(const long seed = -42);
    static sptr make_s16(const long seed = -42);
    static sptr make_s8(const long seed = -42);

    //! Set the waveform type (UNIFORM, GAUSSIAN, LAPLACIAN, IMPULSE)
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

    //! Set the factor, this is used for the IMPULSE waveform
    virtual void set_factor(const double &factor) = 0;

    //! Get the current factor setting
    virtual double get_factor(void) = 0;

};

}

#endif /*INCLUDED_GREXTRAS_NOISE_SOURCE_HPP*/
