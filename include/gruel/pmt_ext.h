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

#ifndef INCLUDED_GRUEL_PMT_EXT_H
#define INCLUDED_GRUEL_PMT_EXT_H

#include <gnuradio/extras/api.h>
#include <gruel/pmt.h>

namespace pmt{

/*
 * ------------------------------------------------------------------------
 *      Binary Large Objects (BLOBs)
 *
 * Handy for passing around uninterpreted chunks of memory.
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a blob, othewise false.
GR_EXTRAS_API bool pmt_is_ext_blob(pmt_t x);

/*!
 * \brief Allocate a blob given the length in bytes
 *
 * \param len is the size of the data in bytes.
 */
GR_EXTRAS_API pmt_t pmt_make_ext_blob(size_t len);

/*!
 * \brief Make a blob given a pointer and length in bytes
 *
 * \param buf is the pointer to data to use to create blob
 * \param buf a pointer to read/write memory.
 * \param len is the size of the data in bytes.
 */
GR_EXTRAS_API pmt_t pmt_make_ext_blob(void *buf, size_t len);

/*!
 * \brief Make a blob given a pointer and length in bytes
 *
 * The data is copied into the blob.
 * \param buf a pointer to read-only memory.
 * \param len is the size of the data in bytes.
 */
GR_EXTRAS_API pmt_t pmt_make_ext_blob(const void *buf, size_t len);

/*!
 * Return a pointer to the blob's data,
 * or NULL if the blob was constructed with read-only memory.
 */
GR_EXTRAS_API void *pmt_ext_blob_rw_data(pmt_t blob);

//! Return a const pointer to the blob's data
GR_EXTRAS_API const void *pmt_ext_blob_ro_data(pmt_t blob);

//! Return the blob's length in bytes
GR_EXTRAS_API size_t pmt_ext_blob_length(pmt_t blob);

//! Set the blob's length in bytes
GR_EXTRAS_API void pmt_ext_blob_set_length(pmt_t blob, size_t len_in_bytes);

}

#endif /* INCLUDED_GRUEL_PMT_EXT_H */
