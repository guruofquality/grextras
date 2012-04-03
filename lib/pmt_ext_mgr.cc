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

#include <gruel/pmt_mgr.h>
#include <queue>
#include <boost/bind.hpp>
#include <gruel/thread.h>

namespace pmt {

static boost::function<void(pmt_base *)> null_deleter;

class pmt_mgr_impl : public pmt_mgr
{
public:
    pmt_mgr_impl(void);
    ~pmt_mgr_impl(void);
    void set(pmt_t x);
    void reset(pmt_t x);
    pmt_t acquire(bool block);
    class mgr_guts;
    typedef boost::shared_ptr<mgr_guts> guts_sptr;
    guts_sptr guts;
};

class pmt_mgr_impl::mgr_guts{
public:
    mgr_guts(void): should_delete(false){}

    bool should_delete;

    std::queue<pmt_t> available;

    //sync mechanisms
    gruel::mutex mutex;
    gruel::condition_variable cond;
};

pmt_mgr_impl::pmt_mgr_impl(void){
    this->guts = pmt_mgr_impl::guts_sptr(new pmt_mgr_impl::mgr_guts());
}

pmt_mgr_impl::~pmt_mgr_impl(void){
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
}

static void mgr_deleter(pmt_mgr_impl::guts_sptr guts, pmt_base *p){
    gruel::scoped_lock lock(guts->mutex);
    if (guts->should_delete){
        pmt_set_deleter(pmt_t(p), null_deleter);
        return;
    }

    guts->available.push(pmt_t(p));
    lock.unlock();
    guts->cond.notify_one();
}

void pmt_mgr_impl::set(pmt_t x)
{
    boost::function<void(pmt_base *)> new_deleter = boost::bind(&mgr_deleter, guts, _1);
    pmt_set_deleter(x, new_deleter);
}

void pmt_mgr_impl::reset(pmt_t x)
{
    pmt_set_deleter(x, null_deleter);
}

pmt_t pmt_mgr_impl::acquire(bool block)
{
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

pmt_mgr::sptr pmt_mgr::make(void){
    return sptr(new pmt_mgr_impl());
}

} //namespace pmt
