// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/hier_block.hpp>
#include <gras/factory.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <iostream>

namespace asio = boost::asio;

static asio::io_service _io_service;

static const long timeout_us = 10*1000; //10ms

#include "udp_socket_message.hpp"
#include "tcp_socket_message.hpp"

gras::HierBlock *make_socket_message(
    const std::string &type,
    const std::string &addr,
    const std::string &port,
    const size_t &mtu
)
{
    if (type == "UDP_SERVER" or type == "UDP_CLIENT")
    {
        return new UDPSocketMessage(type, addr, port, mtu);
    }
    if (type == "TCP_SERVER" or type == "TCP_CLIENT")
    {
        return new TCPSocketMessage(type, addr, port, mtu);
    }
    throw std::runtime_error("SocketMessage::make fail - unknown type " + type);
}

GRAS_REGISTER_FACTORY("/extras/socket_message", make_socket_message)
