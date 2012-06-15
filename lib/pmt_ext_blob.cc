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

#include "pmt_defs.h"
#include <gruel/pmt_blob.h>
#include <boost/shared_array.hpp>
#include <stdexcept>

#define PMT_BLOB_ALIGN_MASK size_t(0x1f) //byte alignment mask of self-allocated blobs

namespace pmt {

static inline std::vector< boost::uint8_t > &extract_vec(pmt_t blob)
{
    return reinterpret_cast<pmt_u8vector *>(blob.get())->d_v;
}

pmt_t pmt_make_blob(size_t len_in_bytes)
{
    pmt_t blob = pmt_make_u8vector(len_in_bytes, 0);
    //maybe not needed, but I wanted to call reserve
    //since it can be resized to sub-max size later
    extract_vec(blob).reserve(len_in_bytes);
    return blob;
}

void pmt_blob_resize(pmt_t blob, size_t len_in_bytes)
{
    //lazy way to ensure the type is correct
    if (!pmt_is_blob(blob))
    {
        throw std::invalid_argument("pmt_blob_resize: got wrong type");
    }
    extract_vec(blob).resize(len_in_bytes);
}

void *pmt_blob_rw_data(pmt_t blob)
{
    return &extract_vec(blob).front();
}

}//namespace pmt
