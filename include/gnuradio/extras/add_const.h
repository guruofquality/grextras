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

#ifndef INCLUDED_GR_EXTRAS_ADD_CONST_H
#define INCLUDED_GR_EXTRAS_ADD_CONST_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>
#include <gr_hier_block2.h>
#include <complex>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API add_const : virtual public gr_hier_block2{
public:
    typedef boost::shared_ptr<add_const> sptr;

    static sptr make_fc32_fc32(const std::complex<double> &val = 0);
    static sptr make_sc32_sc32(const std::complex<double> &val = 0);
    static sptr make_sc16_sc16(const std::complex<double> &val = 0);
    static sptr make_sc8_sc8(const std::complex<double> &val = 0);
    static sptr make_f32_f32(const std::complex<double> &val = 0);
    static sptr make_s32_s32(const std::complex<double> &val = 0);
    static sptr make_s16_s16(const std::complex<double> &val = 0);
    static sptr make_s8_s8(const std::complex<double> &val = 0);

    //! Set the value from any type
    virtual void set_const(const std::complex<double> &val) = 0;

    //! Get the constant value as a complex double
    virtual std::complex<double> get_const(void) = 0;
};

class GR_EXTRAS_API add_const_v : virtual public block{
public:
    typedef boost::shared_ptr<add_const_v> sptr;

    /*!
     * Make an add const from type and const value.
     * The length of value is the vector length of this block.
     */
    static sptr make_fc32_fc32(const std::vector<std::complex<float> > &vec);
    static sptr make_sc32_sc32(const std::vector<std::complex<int32_t> > &vec);
    static sptr make_sc16_sc16(const std::vector<std::complex<int16_t> > &vec);
    static sptr make_sc8_sc8(const std::vector<std::complex<int8_t> > &vec);
    static sptr make_f32_f32(const std::vector<float> &vec);
    static sptr make_s32_s32(const std::vector<int32_t> &vec);
    static sptr make_s16_s16(const std::vector<int16_t> &vec);
    static sptr make_s8_s8(const std::vector<int8_t> &vec);

    //! Set the value from any vector type
    template <typename type>
    void set_const(const std::vector<type> &val);

    //! Get the constant value as a vector of complex double
    virtual std::vector<std::complex<double> > get_const(void) = 0;

private:
    virtual void _set_const(const std::vector<std::complex<double> > &val) = 0;

};

}}

//--- template implementation details below ---//

template <typename type>
void gnuradio::extras::add_const_v::set_const(const std::vector<type> &val){
    std::vector<std::complex<double> > new_val;
    for (size_t i = 0; i < val.size(); i++){
        new_val.push_back(gr_num_to_complex_double(val[i]));
    }
    return this->_set_const(new_val);
}

#endif /* INCLUDED_GR_EXTRAS_ADD_CONST_H */
