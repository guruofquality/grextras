// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SERIALIZER_HPP
#define INCLUDED_GREXTRAS_SERIALIZER_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The serializer block:
 * This block serializes the input stream, messags, and tags;
 * and outputs the serialized data as a PacketMsg message type.
 * The packet message can be sent over a network
 * or possibly -> datagram to stream -> file sink.
 */
struct Serializer
{
    /*!
     * Create a new serialize port block.
     *
     * An MTU of zero means default ethernet payload sizes.
     *
     * The synchronous parameter means that equal amounts of items will
     * be consumed from each port when the serialization is performed.
     *
     * \param mtu max output payload size in bytes
     * \param sync true when input stream are synchronous
     */
    GREXTRAS_API static gras::Block *make(const size_t mtu = 0, const bool sync = true);
};

}


#endif /*INCLUDED_GREXTRAS_SERIALIZER_HPP*/
