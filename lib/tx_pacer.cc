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

        const double rx_time_last = this->nitems_read(1)/_rx_rate;
        const double tx_time_last = this->nitems_read(0)/_tx_rate;

        if (tx_time_last < rx_time_last + _duration)
        {
            const size_t nitems = std::min(output_items[0].size(), input_items[0].size());
            std::memcpy(output_items[0].cast<void *>(), input_items[0].cast<const void *>(), nitems*_tx_size);
            this->consume(0, nitems);
            this->produce(0, nitems);
        }

        //always consume all rx samples input,
        //this gives us a concept of consumed time
        this->consume(1, input_items[1].size());

        return 0;
    }

    size_t _tx_size;
    size_t _rx_rate;
    size_t _tx_rate;
    double _duration;

};

tx_pacer::sptr tx_pacer::make(const size_t tx_size, const size_t rx_size)
{
    return gnuradio::get_initial_sptr(new tx_pacer_impl(tx_size, rx_size));
}
