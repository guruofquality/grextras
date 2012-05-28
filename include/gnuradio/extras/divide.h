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

#ifndef INCLUDED_GR_EXTRAS_DIVIDE_H
#define INCLUDED_GR_EXTRAS_DIVIDE_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API divide : virtual public block{
public:
    typedef boost::shared_ptr<divide> sptr;

    static sptr make_fc32_fc32(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_sc32_sc32(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_sc16_sc16(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_sc8_sc8(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_f32_f32(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_s32_s32(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_s16_s16(const size_t num_inputs, const size_t vlen = 1);
    static sptr make_s8_s8(const size_t num_inputs, const size_t vlen = 1);
};

}}

#endif /* INCLUDED_GR_EXTRAS_DIVIDE_H */
