// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_DATAGRAM_TO_STREAM_HPP
#define INCLUDED_GREXTRAS_DATAGRAM_TO_STREAM_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * Datagram to stream block:
 *
 * The input port is a message port.
 * The output port is a stream of items.
 *
 * Each incoming message is a tag where
 *  - key is set to "datagram"
 *  - the value is a gras::SBuffer
 *
 * If an input message does not match this criteria,
 * it will simply be forwarded to the output port.
 *
 * The implementation of this block is totally zero-copy.
 */
struct GREXTRAS_API Datagram2Stream : virtual gras::Block
{
    typedef boost::shared_ptr<Datagram2Stream> sptr;

    /*!
     * Make a new datagram to stream block.
     * \param itemsize the size of the output port in bytes
     */
    static sptr make(const size_t itemsize);
};

}

#endif /*INCLUDED_GREXTRAS_DATAGRAM_TO_STREAM_HPP*/
