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

#ifndef INCLUDED_GR_EXTRAS_BLOB_TO_FILEDES_H
#define INCLUDED_GR_EXTRAS_BLOB_TO_FILEDES_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API blob_to_filedes : virtual public block{
public:
    typedef boost::shared_ptr<blob_to_filedes> sptr;

    /*!
     * \brief Make a blob to file descriptor block
     *
     * Read incoming blob messages and write to a file descriptor.
     * Non-blob message values will be ignored by the block.
     * An empty blob (length 0 bytes) tells the work to exit.
     *
     * \param fd the already open file descriptor
     * \param close_fd true to close the fd on deconstruction
     * \return a new blob to file descriptor block
     */
    static sptr make(const int fd, const bool close_fd = false);
};

}}

#endif /* INCLUDED_GR_EXTRAS_BLOB_TO_FILEDES_H */
