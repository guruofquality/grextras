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

#ifndef INCLUDED_GR_EXTRAS_STREAM_TO_BLOB_H
#define INCLUDED_GR_EXTRAS_STREAM_TO_BLOB_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API stream_to_blob : virtual public block{
public:
    typedef boost::shared_ptr<stream_to_blob> sptr;

    /*!
     * \brief Make a stream to blob block
     *
     * This block reads bytes from the input stream,
     * and posts blob messages to "blob" subscriber group.
     *
     * \param item_size the size of the input stream in bytes
     * \param mtu the blob length in bytes, or zero for default
     * \return a new stream to blob block
     */
    static sptr make(
        const size_t item_size,
        const size_t mtu = 0
    );

};

}}

#endif /* INCLUDED_GR_EXTRAS_STREAM_TO_BLOB_H */
