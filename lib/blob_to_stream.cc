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

#include <gnuradio/extras/blob_to_stream.h>
#include <gr_io_signature.h>
#include <gruel/pmt_ext.h>
#include <cstring> //std::memcpy

using namespace gnuradio::extras;

class blob_to_stream_impl : public blob_to_stream{
public:
    blob_to_stream_impl(const size_t item_size):
        gr_sync_block(
            "blob_to_stream",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(1, 1, item_size)
        ),
        _item_size(item_size)
    {
        _offset = 0;
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //loop until we get a blob or interrupted
        while (_offset == 0){
            _msg = this->pop_msg_queue();
            if (pmt::pmt_is_ext_blob(_msg.value)) break;
        }
        if (pmt::pmt_ext_blob_length(_msg.value) == 0) return -1; //empty blob, we are done here

        //calculate the number of bytes to copy
        const size_t nblob_items = (pmt::pmt_ext_blob_length(_msg.value) - _offset)/_item_size;
        const size_t noutput_bytes = _item_size*std::min<size_t>(noutput_items, nblob_items);

        //perform memcpy from blob to output items
        const char *blob_mem = reinterpret_cast<const char *>(pmt::pmt_ext_blob_ro_data(_msg.value)) + _offset;
        std::memcpy(output_items[0], blob_mem, noutput_bytes);

        //adjust the offset into the blob memory
        _offset += noutput_bytes;
        if (pmt::pmt_ext_blob_length(_msg.value) == _offset) _offset = 0;

        return noutput_bytes/_item_size;
    }

private:
    const size_t _item_size;
    gr_tag_t _msg;
    size_t _offset;
};

blob_to_stream::sptr blob_to_stream::make(const size_t item_size){
    return blob_to_stream::sptr(new blob_to_stream_impl(item_size));
}
