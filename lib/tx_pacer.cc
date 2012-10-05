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
#include <iostream>

const size_t TX_PORT = 0;
const size_t RX_PORT = 1;

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

    void forecast(int, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[TX_PORT] = 1; //expect at least some TX input
        ninput_items_required[RX_PORT] = 0; //dont care if there is RX input
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){

        const double rx_time_last = this->nitems_read(RX_PORT)/_rx_rate;
        const double tx_time_last = this->nitems_read(TX_PORT)/_tx_rate;

        if (tx_time_last + _duration < rx_time_last)
        {
            const size_t nitems = std::min(output_items[TX_PORT].size(), input_items[TX_PORT].size());
            std::memcpy(output_items[TX_PORT].cast<void *>(), input_items[TX_PORT].cast<const void *>(), nitems*_tx_size);
            this->consume(TX_PORT, nitems);
            this->produce(TX_PORT, nitems);
        }

        //always consume all rx samples input,
        //this gives us a concept of consumed time
        this->consume(RX_PORT, input_items[RX_PORT].size());

        return -2; //work called produce
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
