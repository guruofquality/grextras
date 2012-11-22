// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/socket_message.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <gras/block.hpp>
#include <stdexcept>
#include <iostream>

namespace asio = boost::asio;
using namespace grextras;

static const PMCC DATAGRAM_KEY = PMC::make(std::string("datagram"));

/***********************************************************************
 * cross platform select code for socket
 **********************************************************************/
static const long timeout_us = 100*1000; //100ms

static bool wait_for_recv_ready(int sock_fd)
{
    //setup timeval for timeout
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us;

    //setup rset for timeout
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(sock_fd, &rset);

    //call select with timeout on receive socket
    return ::select(sock_fd+1, &rset, NULL, NULL, &tv) > 0;
}

#include "udp_socket_message.hpp"
#include "tcp_socket_message.hpp"

SocketMessage::sptr SocketMessage::make(
    const std::string &type,
    const std::string &addr,
    const std::string &port,
    const size_t mtu
)
{
    if (type == "UDP_SERVER" or type == "UDP_CLIENT")
    {
        return boost::make_shared<UDPSocketMessage>(type, addr, port, mtu);
    }
    if (type == "TCP_SERVER" or type == "TCP_CLIENT")
    {
        return boost::make_shared<TCPSocketMessage>(type, addr, port, mtu);
    }
    throw std::runtime_error("SocketMessage::make fail - unknown type " + type);
}
