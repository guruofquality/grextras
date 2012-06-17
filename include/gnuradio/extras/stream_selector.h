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

#ifndef INCLUDED_GR_EXTRAS_STREAM_SELECTOR_H
#define INCLUDED_GR_EXTRAS_STREAM_SELECTOR_H

#include <gnuradio/extras/api.h>
#include <gr_hier_block2.h>

namespace gnuradio{ namespace extras{

/*!
 * The sector extras allows streams to be dynamically routed at runtime.
 */
class GR_EXTRAS_API stream_selector : virtual public gr_hier_block2{
public:
    typedef boost::shared_ptr<stream_selector> sptr;

    /*!
     * Make a new stream selector block
     * \param in_sig signature to describe inputs
     * \param out_sig signature to describe outputs
     */
    static sptr make(gr_io_signature_sptr in_sig, gr_io_signature_sptr out_sig);

    /*!
     * Set the path for samples for each input ports.
     *
     * Paths should be number of input ports in length.
     * Each element represents the destination for an input port.
     * The possible values for each element is an output port index,
     * or the value may also be: -1 to block or -2 to consume.
     *
     * Ex, a block with one input and two outputs:
     * paths = [1] input0 -> output1, output0 nothing.
     *
     * Ex, a block with two inputs and one output:
     * paths = [-1, 0] input0 blocks, input1 -> output0.
     *
     * \param paths a list of stream destinations for each port
     */
    virtual void set_paths(const std::vector<int> &paths) = 0;

};

}}

#endif /* INCLUDED_GR_EXTRAS_STREAM_SELECTOR_H */
