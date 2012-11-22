// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_STREAM_TO_DATAGRAM_HPP
#define INCLUDED_GREXTRAS_STREAM_TO_DATAGRAM_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * Stream to datagram block:
 *
 * The input port is a stream of items.
 * The output port is a message port.
 *
 * Each outgoing message is a tag where
 *  * key is set to "datagram"
 *  * the value is a gras::SBuffer
 *
 * For each call of work(), the default behaviour is to post 
 * the entire input buffer as the value of the output tag.
 *
 * The user may override this by setting a non-zero mtu;
 * so that the work function will slice up the input buffer.
 *
 * If the input stream also contains item tags as well,
 * these tags will also be posted to the output port.
 */
struct GREXTRAS_API Stream2Datagram : virtual gras::Block
{
    typedef boost::shared_ptr<Stream2Datagram> sptr;

    /*!
     * Make a new stream to datagram block.
     * An MTU of zero disables slicing of input buffers.
     * \param itemsize the size of the input port in bytes
     * \param mtu the MTU in bytes for the slicing logic.
     */
    static sptr make(const size_t itemsize, const size_t mtu = 0);
};

}

#endif /*INCLUDED_GREXTRAS_STREAM_TO_DATAGRAM_HPP*/
