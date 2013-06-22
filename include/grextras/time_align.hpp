// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_TIME_ALIGN_HPP
#define INCLUDED_GREXTRAS_TIME_ALIGN_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>

namespace grextras
{

/*!
 * The time alignment block inputs asynchronous RX streams,
 * and outputs time aligned synchronous RX streams.
 * This block could be used to align multiple USRP sources.
 *
 * The time alignment block is meant to be robust against
 * stream restarts, packet dropouts, and stream overflows.
 * However, the alignment recovery logic of this block
 * demands that all input streams start out aligned,
 * because the logic looks for matching time tags.
 *
 * Time tag format: gras::StreamTag, where key is "rx_time",
 * and val is a PMCTuple<2>(int64 full_seconds, double frac_secs)
 */
struct GREXTRAS_API TimeAlign : virtual gras::Block
{
    typedef boost::shared_ptr<TimeAlign> sptr;

    /*!
     * Make a new TimeAlign block.
     * The itemsize is the size of a sample in bytes.
     */
    static sptr make(const size_t itemsize);
};

}

#endif /*INCLUDED_GREXTRAS_TIME_ALIGN_HPP*/
