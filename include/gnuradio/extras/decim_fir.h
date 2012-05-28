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

#ifndef INCLUDED_GR_EXTRAS_DECIM_FIR_H
#define INCLUDED_GR_EXTRAS_DECIM_FIR_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>
#include <vector>
#include <complex>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API decim_fir : virtual public block{
public:
    typedef boost::shared_ptr<decim_fir> sptr;
    typedef std::vector<std::complex<float> > taps_type;

    //! Make a new FIR filter complex float input, taps, output type
    static sptr make_fc32_fc32_fc32(
        const taps_type &taps,
        const size_t decim = 1
    );

    //! Make a new FIR filter float input, taps, output type
    static sptr make_f32_f32_f32(
        const taps_type &taps,
        const size_t decim = 1
    );

    //! Set the taps for this FIR filter
    virtual void set_taps(const taps_type &taps) = 0;

    //! Get the taps set on this FIR filter
    virtual taps_type get_taps(void) = 0;

};

}}

#endif /* INCLUDED_GR_EXTRAS_DECIM_FIR_H */
