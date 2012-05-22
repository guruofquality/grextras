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

#include <gnuradio/extras/stream_to_blob.h>
#include <gr_io_signature.h>
#include <cstring> //std::memcpy
#include <stdexcept>

using namespace gnuradio::extras;

static const pmt::pmt_t BLOB_KEY = pmt::pmt_string_to_symbol("blob_stream");
static const size_t POOL_SIZE = 64; //num pre-allocated blobs to acquire at once

class stream_to_blob_impl : public stream_to_blob{
public:
    stream_to_blob_impl(
        const size_t item_size,
        const size_t mtu,
        const bool fixed
    ):
        block(
            "stream_to_blob",
            gr_make_io_signature(1, 1, item_size),
            gr_make_io_signature(0, 0, 0),
            msg_signature(false, 1)
        ),
        _item_size(item_size),
        _mtu(mtu),
        _fixed(fixed)
    {
        if (mtu%item_size != 0) throw std::invalid_argument(
            "stream_to_blob MTU is not a multiple of item size"
        );

        if (fixed) this->set_output_multiple(mtu/item_size);

        std::stringstream str;
        str << name() << unique_id();
        _id = pmt::pmt_string_to_symbol(str.str());

        //pre-allocate blobs
        _mgr = pmt::pmt_mgr::make();
        for (size_t i = 0; i < POOL_SIZE; i++){
            _mgr->set(pmt::pmt_make_blob(mtu));
        }
    }

    bool stop(void){
        //post an empty blob to mark stop
        //this is used in the blob qa code to cause the blob to stream to exit work
        pmt::pmt_t blob = pmt::pmt_make_blob(0);
        this->post_msg(0, BLOB_KEY, blob, _id);
        return true;
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        size_t ninput_items = input_items[0].size();

        if (_fixed && ninput_items*_item_size < _mtu){
            throw std::runtime_error("stream to blob made false assumption about set_output_multiple");
        }

        //cap the output items to the mtu size
        ninput_items = std::min<size_t>(ninput_items, _mtu/_item_size);

        //acquire blob and memcpy stream memory to the blob memory
        pmt::pmt_t blob = _mgr->acquire(true /*block*/);
        pmt::pmt_blob_resize(blob, ninput_items*_item_size);
        std::memcpy(pmt::pmt_blob_rw_data(blob), input_items[0].get(), pmt::pmt_blob_length(blob));

        //post the message to downstream subscribers
        this->post_msg(0, BLOB_KEY, blob, _id);

        //yield the number of consumed items
        return ninput_items;
    }

private:
    const size_t _item_size;
    const size_t _mtu;
    const bool _fixed;
    pmt::pmt_t _id;
    pmt::pmt_mgr::sptr _mgr;
};

stream_to_blob::sptr stream_to_blob::make(
    const size_t item_size,
    const size_t mtu_
){
    const size_t mtu = (mtu_ == 0)? 2048 : mtu_;
    const bool fixed = (mtu_ != 0);
    return gnuradio::get_initial_sptr(new stream_to_blob_impl(item_size, mtu, fixed));
}
