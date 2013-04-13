// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_DESERIALIZE_PORT_HPP
#define INCLUDED_GREXTRAS_DESERIALIZE_PORT_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The deserialize port block:
 * This block deserializes the input PacketMsg message type;
 * and outputs the data as stream, messags, and tags.
 * The packet message can be received from a network
 * or possibly from file source -> stream to datagram.
 */
struct GREXTRAS_API DeserializePort : virtual gras::Block
{
    typedef boost::shared_ptr<DeserializePort> sptr;

    /*!
     * Create a new deserialize port block.
     */
    static sptr make(void);
};

}


#endif /*INCLUDED_GREXTRAS_DESERIALIZE_PORT_HPP*/
