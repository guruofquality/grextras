/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
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

#ifndef INCLUDED_GR_EXTRAS_SOCKET_TO_BLOB_H
#define INCLUDED_GR_EXTRAS_SOCKET_TO_BLOB_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API socket_to_blob : virtual public block{
public:
    typedef boost::shared_ptr<socket_to_blob> sptr;

    /*!
     * \brief Make a message blob to socket block
     *
     * The UDP socket will be bound to the specified addr/port.
     * The TCP socket will listen on addr/port, and accept the first connection.
     *
     * This block reads packets from a UDP socket,
     * and posts blob messages to "blob" subscriber group.
     *
     * \param proto the protocol "UDP" or "TCP"
     * \param addr the resolvable interface address of the UDP socket
     * \param port the resolvable interface port of the UDP socket
     * \param mtu the max bytes in an incoming packet, 0 for default
     * \return a new socket to blob block
     */
    static sptr make(
        const std::string &proto, const std::string &addr, const std::string &port, const size_t mtu = 0
    );

};

}}

#endif /* INCLUDED_GR_EXTRAS_SOCKET_TO_BLOB_H */
