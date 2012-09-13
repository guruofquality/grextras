/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_EXTRAS_SOCKET_MSG_H
#define INCLUDED_GR_EXTRAS_SOCKET_MSG_H

#include <gnuradio/extras/api.h>
#include <gr_hier_block2.h>

namespace gnuradio{ namespace extras{

class GR_EXTRAS_API socket_msg : virtual public gr_hier_block2{
public:
    typedef boost::shared_ptr<socket_msg> sptr;

    /*!
     * \brief Make a socket block with message blobs in and out
     *
     * The TCP socket will listen on addr/port, and accept the first connection.
     *
     * \param proto the protocol "TCP" only for now
     * \param addr the resolvable interface address of the socket
     * \param port the resolvable interface port of the socket
     * \param mtu the max bytes in an incoming packet, 0 for default
     * \return a new socket message block
     */
    static sptr make(
        const std::string &proto, const std::string &addr, const std::string &port, const size_t mtu = 0
    );

};

}}

#endif /* INCLUDED_GR_EXTRAS_SOCKET_MSG_H */
