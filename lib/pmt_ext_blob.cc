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

#include <gruel/pmt_ext.h>
#include <boost/shared_array.hpp>

#define PMT_BLOB_ALIGN_MASK size_t(0x1f) //byte alignment mask of self-allocated blobs

namespace pmt {

class pmt_blob
{
public:
    typedef boost::shared_array<char> shart;
    pmt_blob(const void *, void *, size_t, shart);

    const void *romem;
    void *rwmem;
    size_t len;
    shart shar;
};

pmt_blob::pmt_blob(const void *rom, void *rwm, size_t l, shart s = shart()):
    romem(rom), rwmem(rwm), len(l), shar(s){}

bool pmt_is_ext_blob(pmt_t obj)
{
    if (!pmt_is_any(obj)) return false;
    try
    {
        boost::any_cast<pmt_blob>(pmt_any_ref(obj));
    }
    catch(const boost::bad_any_cast &)
    {
        return false;
    }
    return true;
}

pmt_t pmt_make_ext_blob(size_t len)
{
    pmt_blob::shart shar(new char[len + PMT_BLOB_ALIGN_MASK]);
    const size_t addr = (size_t(shar.get()) + PMT_BLOB_ALIGN_MASK) & ~PMT_BLOB_ALIGN_MASK;
    char *mem = reinterpret_cast<char *>(addr);
    pmt_blob blob(mem, mem, len, shar);
    return pmt_make_any(blob);
}

pmt_t pmt_make_ext_blob(void *buf, size_t len)
{
    pmt_blob blob(buf, buf, len);
    return pmt_make_any(blob);
}

pmt_t pmt_make_ext_blob(const void *buf, size_t len)
{
    pmt_blob blob(buf, NULL, len);
    return pmt_make_any(blob);
}

void *pmt_ext_blob_rw_data(pmt_t blob)
{
    if (!pmt_is_ext_blob(blob))
        throw pmt_wrong_type("pmt_blob_rw_data", blob);
    return boost::any_cast<pmt_blob>(pmt_any_ref(blob)).rwmem;
}

const void *pmt_ext_blob_ro_data(pmt_t blob)
{
    if (!pmt_is_ext_blob(blob))
        throw pmt_wrong_type("pmt_blob_ro_data", blob);
    return boost::any_cast<pmt_blob>(pmt_any_ref(blob)).romem;
}

size_t pmt_ext_blob_length(pmt_t blob)
{
    if (!pmt_is_ext_blob(blob))
        throw pmt_wrong_type("pmt_blob_length", blob);
    return boost::any_cast<pmt_blob>(pmt_any_ref(blob)).len;
}

void pmt_ext_blob_set_length(pmt_t blob, size_t len_in_bytes)
{
    if (!pmt_is_ext_blob(blob))
        throw pmt_wrong_type("pmt_blob_set_length", blob);
    pmt_blob b = boost::any_cast<pmt_blob>(pmt_any_ref(blob));
    b.len = len_in_bytes;
    pmt_any_set(blob, b);
}

}//namespace pmt
