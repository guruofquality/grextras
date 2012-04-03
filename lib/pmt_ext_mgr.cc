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
#include <queue>
#include <boost/bind.hpp>
#include <gruel/thread.h>

namespace pmt {

static boost::function<void(pmt_base *)> null_deleter;

class pmt_mgr
{

public:
  pmt_mgr(void);

  class mgr_guts;
  typedef boost::shared_ptr<mgr_guts> guts_sptr;
  guts_sptr guts;

  bool is_mgr() const { return true; }
};

class pmt_mgr::mgr_guts{
public:
  mgr_guts(void): should_delete(false){}

  bool should_delete;

  std::queue<pmt_t> available;

  //sync mechanisms
  gruel::mutex mutex;
  gruel::condition_variable cond;
};

pmt_mgr::pmt_mgr(void){
    this->guts = pmt_mgr::guts_sptr(new pmt_mgr::mgr_guts());
}

static void mgr_master_deleter(pmt_mgr::guts_sptr guts, pmt_base *p){
    //tells the guts to delete all returning pmts
    //frees any already returned pmts in guts
    //guts will deconstruct after it frees all bound pmts
    gruel::scoped_lock lock(guts->mutex);
    guts->should_delete = true;
    while(!guts->available.empty()){
        lock.unlock();
        guts->available.pop();
        lock.lock();
    }
    pmt_set_deleter(pmt_t(p), null_deleter);
}

static void mgr_deleter(pmt_mgr::guts_sptr guts, pmt_base *p){
    gruel::scoped_lock lock(guts->mutex);
    if (guts->should_delete){
        pmt_set_deleter(pmt_t(p), null_deleter);
        return;
    }

    guts->available.push(pmt_t(p));
    lock.unlock();
    guts->cond.notify_one();
}

bool pmt_is_mgr(pmt_t obj)
{
    if (!pmt_is_any(obj)) return false;
    try
    {
        boost::any_cast<pmt_mgr>(pmt_any_ref(obj));
    }
    catch(const boost::bad_any_cast &)
    {
        return false;
    }
    return true;
}

pmt_t pmt_make_mgr(void)
{
    pmt_mgr mgr = pmt_mgr();
    pmt_t m = pmt_make_any(mgr);
    boost::function<void(pmt_base *)> new_deleter = boost::bind(&mgr_master_deleter, mgr.guts, _1);
    pmt_set_deleter(m, new_deleter);
    return m;
}

void
pmt_mgr_set(pmt_t mgr, pmt_t x)
{
    if (!pmt_is_mgr(mgr))
        throw pmt_wrong_type("pmt_mgr_set", mgr);
    pmt_mgr m = boost::any_cast<pmt_mgr>(pmt_any_ref(mgr));
    boost::function<void(pmt_base *)> new_deleter = boost::bind(&mgr_deleter, m.guts, _1);
    pmt_set_deleter(x, new_deleter);
}

void
pmt_mgr_reset(pmt_t mgr, pmt_t x)
{
    if (!pmt_is_mgr(mgr))
        throw pmt_wrong_type("pmt_mgr_reset", mgr);
    pmt_mgr m = boost::any_cast<pmt_mgr>(pmt_any_ref(mgr));
    pmt_set_deleter(x, null_deleter);
}

pmt_t
pmt_mgr_acquire(pmt_t mgr, bool block)
{
    if (!pmt_is_mgr(mgr))
        throw pmt_wrong_type("pmt_mgr_acquire", mgr);

    pmt_mgr m = boost::any_cast<pmt_mgr>(pmt_any_ref(mgr));
    pmt_mgr::guts_sptr guts = m.guts;

    gruel::scoped_lock lock(guts->mutex);
    std::queue<pmt_t> &available = guts->available;

    while (available.empty()){
        if (block) guts->cond.wait(lock);
        else return PMT_NIL;
    }

    pmt_t p = available.front();
    available.pop();
    return p;
}

} //namespace pmt
