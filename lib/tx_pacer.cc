/*
 * Copyright 2012 Ettus Research LLC
 * 
 * This file is part of GR Extras
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

#include <gnuradio/extras/tx_pacer.h>
#include <gr_io_signature.h>
#include <boost/foreach.hpp>

static const pmt::pmt_t TIME_KEY = pmt::pmt_string_to_symbol("rx_time");

using namespace gnuradio::extras;

struct tx_pacer_impl : tx_pacer
{
    tx_pacer_impl(const size_t tx_size, const size_t rx_size):
        gnuradio::block(
            "tx pacer",
            gr_make_io_signature2(2, 2, tx_size, rx_size),
            gr_make_io_signature(1, 1, tx_size)
        )
    {
        this->set_auto_consume(false);
        this->set_tx_sample_rate(1.0);
        this->set_rx_sample_rate(1.0);
        this->set_tx_window_duration(1.0);

        _tx_size = tx_size;
        _rx_time_offset = 0;
        _rx_time_at_offset = 0.0;
        _tx_time_offset = 0;
        _tx_time_at_offset = 0.0;
    }

    void set_tx_sample_rate(const double rate)
    {
        _tx_rate = rate;
    }

    void set_rx_sample_rate(const double rate)
    {
        _rx_rate = rate;
    }

    void set_tx_window_duration(const double duration)
    {
        _duration = duration;
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        /***************************************************************
         * Step 0)
         * Consume and process the RX input port (port 1).
         * Time tags + consum -> update this block's concept of time.
         **************************************************************/
        //read all tags associated with port 1 for items in this work function
        const uint64_t nread = this->nitems_read(1); //number of items read on port 1
        this->get_tags_in_range(_tags, 1, nread, nread+input_items[1].size());
        BOOST_FOREACH(const gr_tag_t &tag, _tags)
        {
            if (pmt::pmt_equal(tag.key, TIME_KEY))
            {
                _rx_time_offset = tag.offset;
                uint64_t time_integer = pmt::pmt_to_uint64(pmt_tuple_ref(tag.value, 0));
                double time_fractional = pmt::pmt_to_double(pmt_tuple_ref(tag.value, 1));
                //double(time_integer) throws out some precision, this is approximate
                _rx_time_at_offset = double(time_integer) + time_fractional;
            }
        }
        this->consume(1, input_items[1].size()); //consume port 1 input

        /***************************************************************
         * Step 1)
         * Calculate the time of the very last RX sample consumed
         **************************************************************/
        const double rx_time_last = _rx_time_at_offset + (nread+input_items[1].size() - _rx_time_offset)/_rx_rate;
        const double tx_time_last = _tx_time_at_offset + (this->nitems_read(0) - _tx_time_offset)/_tx_rate;

        if (tx_time_last < rx_time_last + _duration)
        {
            _tx_time_offset = this->nitems_read(0);
            _tx_time_at_offset = tx_time_last;

            const size_t nitems = std::min(output_items[0].size(), input_items[0].size());
            std::memcpy(output_items[0].cast<void *>(), input_items[0].cast<const void *>(), nitems*_tx_size);
            this->consume(0, nitems);
            this->produce(0, nitems);
        }

        return 0;
    }

    size_t _tx_size;
    size_t _rx_rate;
    size_t _tx_rate;
    double _duration;
    std::vector<gr_tag_t> _tags;

    uint64_t _rx_time_offset;
    double _rx_time_at_offset;

    uint64_t _tx_time_offset;
    double _tx_time_at_offset;

};

tx_pacer::sptr tx_pacer::make(const size_t tx_size, const size_t rx_size)
{
    return gnuradio::get_initial_sptr(new tx_pacer_impl(tx_size, rx_size));
}
