// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_DESERIALIZER_HPP
#define INCLUDED_GREXTRAS_DESERIALIZER_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The deserializer block:
 * This block deserializes the input PacketMsg message type;
 * and outputs the data as stream, messags, and tags.
 * The packet message can be received from a network
 * or possibly from file source -> stream to datagram.
 */
struct Deserializer
{
    /*!
     * Create a new deserialize port block.
     * The recovery parameter allows the block
     * to recover packet boundaries from streams or files.
     * \param recover true to recover packet boundaries
     */
    GREXTRAS_API static gras::Block *make(const bool recover = true);
};

}


#endif /*INCLUDED_GREXTRAS_DESERIALIZER_HPP*/
