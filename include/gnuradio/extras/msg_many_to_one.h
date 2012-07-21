/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_EXTRAS_MSG_MANY_TO_ONE_H
#define INCLUDED_GR_EXTRAS_MSG_MANY_TO_ONE_H

#include <gnuradio/extras/api.h>
#include <gr_hier_block2.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API msg_many_to_one : virtual public gr_hier_block2{
public:
    typedef boost::shared_ptr<msg_many_to_one> sptr;

    static sptr make(const size_t num_inputs);
};

}}

#endif /* INCLUDED_GR_EXTRAS_MSG_MANY_TO_ONE_H */
