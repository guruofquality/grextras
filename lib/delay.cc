/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <gnuradio/extras/delay.h>
#include <gr_io_signature.h>
#include <cstring> //memcpy
#include <gruel/thread.h>

using namespace gnuradio::extras;

/***********************************************************************
 * Generic delay implementation
 **********************************************************************/
class delay_impl : public delay{
public:
    delay_impl(const size_t itemsize):
        block(
            "extras delay block",
            gr_make_io_signature (1, 1, itemsize),
            gr_make_io_signature (1, 1, itemsize)
        ),
        _itemsize(itemsize)
    {
        this->set_auto_consume(false);
        this->set_delay(0);
    }

    void set_delay(const int nitems){
        gruel::scoped_lock l(_delay_mutex);
        _delay_items = -nitems;
    }

    void forecast(
        int noutput_items,
        gr_vector_int &ninput_items_required
    ){
        //simple 1:1 ratio forecast
        for (unsigned i = 0; i < ninput_items_required.size(); i++)
        {
            ninput_items_required[i] = noutput_items;
        }
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        gruel::scoped_lock l(_delay_mutex);
        size_t noutput_items = output_items[0].size();
        const int delta = int64_t(nitems_read(0)) - int64_t(nitems_written(0)) - _delay_items;

        //consume but not produce (drops samples)
        if (delta < 0){
            this->consume_each(std::min(input_items[0].size(), size_t(-delta)));
            return 0;
        }

        //produce but not consume (inserts zeros)
        if (delta > 0){
            noutput_items = std::min(noutput_items, size_t(delta));
            std::memset(output_items[0].get(), 0, output_items[0].size()*_itemsize);
            return noutput_items;
        }

        //otherwise just memcpy
        noutput_items = std::min(noutput_items, input_items[0].size());
        std::memcpy(output_items[0].get(), input_items[0].get(), noutput_items*_itemsize);
        consume_each(noutput_items);
        return noutput_items;
    }

private:
    int _delay_items;
    const size_t _itemsize;
    gruel::mutex _delay_mutex;
};

/***********************************************************************
 * Delay factory function
 **********************************************************************/
delay::sptr delay::make(const size_t itemsize){
    return gnuradio::get_initial_sptr(new delay_impl(itemsize));
}
