/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FILEDES_TO_BLOB_H
#define INCLUDED_GR_FILEDES_TO_BLOB_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_filedes_to_blob;

/*!
 * \brief Make a file descriptor to blob block
 *
 * Read packets/bytes from a file descriptor
 * and post blob messages to the "blob" subscriber group.
 *
 * \param fd the already open file descriptor
 * \param mtu the max bytes for a read, 0 for default
 * \param close_fd true to close the fd on deconstruction
 * \return a new file descriptor to blob block
 */
GR_CORE_API boost::shared_ptr<gr_filedes_to_blob> gr_make_filedes_to_blob(
    const int fd, const size_t mtu = 0, const bool close_fd = false
);

class GR_CORE_API gr_filedes_to_blob : virtual public gr_sync_block{
};

#endif /* INCLUDED_GR_FILEDES_TO_BLOB_H */
