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

#include <gnuradio/extras/uhd_amsg_source.h>
#include <gr_io_signature.h>
#include <boost/thread/thread.hpp>

using namespace gnuradio::extras;

static const pmt::pmt_t AMSG_KEY = pmt::pmt_string_to_symbol("uhd_amsg");
static const pmt::pmt_t TIME_KEY = pmt::pmt_string_to_symbol("timestamp");
static const pmt::pmt_t CHAN_KEY = pmt::pmt_string_to_symbol("channel");
static const pmt::pmt_t EVENT_KEY = pmt::pmt_string_to_symbol("event_code");

/***********************************************************************
 * UHD Asynchronous Message Source Impl
 **********************************************************************/
class uhd_amsg_source_impl : public uhd_amsg_source{
public:
    uhd_amsg_source_impl(
        const uhd::device_addr_t &device_addr
    ):
        block(
            "gr uhd amsg source",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(false, 1)
        )
    {
        _dev = uhd::usrp::multi_usrp::make(device_addr);
        std::stringstream str;
        str << name() << unique_id();
        _id = pmt::pmt_string_to_symbol(str.str());
    }

    int work(
        const InputItems &,
        const OutputItems &
    ){
        uhd::async_metadata_t md;
        //pop messages from uhd and post to the subscribers
        while (!boost::this_thread::interruption_requested()){
            if (_dev->get_device()->recv_async_msg(md, 0.1)){
                this->post_async_md(md);
            }
        }
        return -1; //done running, work done status
    }

    void post_async_md(const uhd::async_metadata_t &md){
        pmt::pmt_t value_dict = pmt::pmt_make_dict();
        if (md.has_time_spec){
            pmt::pmt_t timestamp = pmt::pmt_make_tuple(
                pmt::pmt_from_uint64(md.time_spec.get_full_secs()),
                pmt::pmt_from_double(md.time_spec.get_frac_secs())
            );
            value_dict = pmt::pmt_dict_add(value_dict, TIME_KEY, timestamp);
        }
        value_dict = pmt::pmt_dict_add(value_dict, CHAN_KEY, pmt::pmt_from_uint64(md.channel));
        value_dict = pmt::pmt_dict_add(value_dict, EVENT_KEY, pmt::pmt_from_uint64(md.event_code));
        this->post_msg(0, AMSG_KEY, value_dict, _id);
    }

protected:
    uhd::usrp::multi_usrp::sptr _dev;
    pmt::pmt_t _id;
};

/***********************************************************************
 * Make UHD Asynchronous Message Source
 **********************************************************************/
uhd_amsg_source::sptr uhd_amsg_source::make(const uhd::device_addr_t &device_addr){
    return gnuradio::get_initial_sptr(new uhd_amsg_source_impl(device_addr));
}
