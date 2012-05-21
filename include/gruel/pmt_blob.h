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

#ifndef INCLUDED_GRUEL_PMT_BLOB_H
#define INCLUDED_GRUEL_PMT_BLOB_H

#include <gnuradio/extras/api.h>
#include <gruel/pmt.h>

namespace pmt{

/*
 * ------------------------------------------------------------------------
 *      Binary Large Objects (BLOBs)
 *
 * Handy for passing around uninterpreted chunks of memory.
 * Below are additions to the blob* family of PMT functions.
 * ------------------------------------------------------------------------
 */

//! Make a new pmt blob, with uninitialized data
GR_EXTRAS_API pmt_t pmt_make_blob(size_t len_in_bytes);

//! Resize the buffer in this blob to a new size in bytes
GR_EXTRAS_API void pmt_blob_resize(pmt_t blob, size_t len_in_bytes);

//! Get access to an writable pointer from this blob
GR_EXTRAS_API void *pmt_blob_rw_data(pmt_t blob);

}

#endif /* INCLUDED_GRUEL_PMT_BLOB_H */
