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

#ifndef INCLUDED_GR_EXTRAS_UHD_AMSG_SOURCE_H
#define INCLUDED_GR_EXTRAS_UHD_AMSG_SOURCE_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>
#include <uhd/usrp/multi_usrp.hpp>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API uhd_amsg_source : virtual public block{
public:
    typedef boost::shared_ptr<uhd_amsg_source> sptr;

    /*!
     * \brief Make a new USRP asynchronous message-based source block.
     * \ingroup uhd_blk
     *
     * The async message source will post messages to the "async" subscriber group.
     * The key of each message will be the pmt symbol: "uhd_amsg"
     * The value of each message will be a pmt dictionary with the following keys/values:
     *  - symbol "timestamp" -> tuple(uint64 seconds, and double fractional seconds)
     *  - sybol "channel" -> uint64 channel number
     *  - sybol "event_code" -> uint64 event code
     */
    static sptr make(const uhd::device_addr_t &device_addr);
};

}}

#endif /* INCLUDED_GR_EXTRAS_UHD_AMSG_SOURCE_H */
