// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SOCKET_MESSAGE_HPP
#define INCLUDED_GREXTRAS_SOCKET_MESSAGE_HPP

#include <grextras/config.hpp>
#include <gras/hier_block.hpp>
#include <string>

namespace grextras
{

/*!
 * Socket message block:
 *
 * This block provides input and output message ports
 * that connect up directly with an internal socket.
 * The socket can be a UDP/TCP client/server.
 *
 * The input port is a message port.
 * Each incoming message is a tag where
 *  - key is set to "datagram"
 *  - the value is a gras::SBuffer
 * Other Input messages are dropped.
 *
 * The output port is a message port.
 * Each outgoing message is a tag where
 *  - key is set to "datagram"
 *  - the value is a gras::SBuffer
 *
 * The following socket types are supported:
 *
 * UDP_CLIENT - a UDP socket that calls connect.
 * This socket initiates outbound communication
 * before inbound communication is possible.
 *
 * UDP_SERVER - a UDP socket that calls bind.
 * This socket's outbound destination is
 * the source of the last inbound packet.
 *
 * TCP_CLIENT - a TCP socket that calls connect.
 * This option expects the TCP server to be running.
 *
 * TCP_SERVER - a TCP socket thats listens and accepts.
 * This option continually listens for connections.
 * That last connection established is the one used.
 */
struct GREXTRAS_API SocketMessage : virtual gras::HierBlock
{
    typedef boost::shared_ptr<SocketMessage> sptr;

    /*!
     * Make a new socket message block.
     * Possible values for the type argument are:
     * "UDP_CLIENT", "UDP_SERVER", "TCP_CLIENT", "TCP_SERVER"
     * \param type the type of socket used in this block
     * \param addr the resolvable address for the socket
     * \param port the resolvable port for the socket
     * \param mtu the MTU in bytes for max rx pkt size
     */
    static sptr make(
        const std::string &type,
        const std::string &addr,
        const std::string &port,
        const size_t mtu = 1500
    );
};

}

#endif /*INCLUDED_GREXTRAS_SOCKET_MESSAGE_HPP*/
