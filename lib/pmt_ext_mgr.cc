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
#include <gruel/pmt_mgr.h>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <set>

namespace pmt {

class pmt_mgr_impl : public pmt_mgr
{
public:

    pmt_mgr_impl(void)
    {
        //NOP
    }

    ~pmt_mgr_impl(void)
    {
        _managed_pmts.clear();
    }

    void set(pmt_t x)
    {
        _managed_pmts.insert(x);
    }

    void reset(pmt_t x)
    {
        _managed_pmts.erase(x);
    }

    pmt_t acquire(bool block = true)
    {
        //its a lazy spin implementation, best I can do without gr core mods
        //a count of 1 means we are the only ones holding the PMT reference
        do{
            BOOST_FOREACH(const pmt_t &p, _managed_pmts)
            {
                if (p->count_ == 1) return p;
            }
            boost::this_thread::yield();
            boost::this_thread::interruption_point();
        } while (block);
        return PMT_NIL;
    }

private:
    std::set<pmt_t> _managed_pmts;
};

pmt_mgr::sptr pmt_mgr::make(void)
{
    return boost::make_shared<pmt_mgr_impl>();
}


} //namespace pmt
