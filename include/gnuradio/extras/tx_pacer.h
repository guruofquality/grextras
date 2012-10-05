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

#ifndef INCLUDED_GR_EXTRAS_TX_PACER_H
#define INCLUDED_GR_EXTRAS_TX_PACER_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>

namespace gnuradio{ namespace extras{

/*!
 * The TX pacer block uses a continuous RX stream
 * to throttle a continuous stream of TX samples.
 *
 * Input port 0 - TX input
 * Input port 1 - RX input (reads time rx_time tags)
 * Output port 0 - TX output
 */
class GR_EXTRAS_API tx_pacer : virtual public block{
public:
    typedef boost::shared_ptr<tx_pacer> sptr;

    /*!
     * Make a new TX pacer block.
     * \param tx_stream_item_size bytes per sample on TX stream
     * \param rx_stream_item_size bytes per sample on RX stream
     */
    static sptr make(
        const size_t tx_stream_item_size = sizeof(std::complex<float>),
        const size_t rx_stream_item_size = sizeof(std::complex<float>)
    );

    //! Set the transmit sample rate in Sps
    virtual void set_tx_sample_rate(const double rate) = 0;

    //! Set the receive sample rate in Sps
    virtual void set_rx_sample_rate(const double rate) = 0;

    /*!
     * Set the transmit window duration.
     * How many seconds worth of TX samples can be in transit?
     * \param duration duration in seconds
     */
     virtual void set_tx_window_duration(const double duration) = 0;
};

}}

#endif /* INCLUDED_GR_EXTRAS_TX_PACER_H */
