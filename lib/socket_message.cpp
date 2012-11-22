// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/socket_message.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <gras/block.hpp>
#include <stdexcept>

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

/***********************************************************************
 * Receive from a UDP socket and post tag to output
 **********************************************************************/
struct UDPSocketReceiver : gras::Block
{
    UDPSocketReceiver(const size_t mtu):
        gras::Block("GrExtras UDPSocketReceiver"),
        _mtu(mtu)
    {
        this->set_output_signature(gras::IOSignature(1));
        //TODO allocate pool
    }

    void work(const InputItems &, const OutputItems &)
    {
        //wait for a packet to become available
        if (not wait_for_recv_ready(socket->native())) return;

        //TODO use pool
        gras::SBufferConfig config;
        config.length = _mtu;
        gras::SBuffer b(config);

        //receive into the buffer
        b.length = socket->receive_from(asio::buffer(b.get(), b.get_actual_length()), *endpoint);

        //create a tag for this buffer
        const gras::Tag t(0, DATAGRAM_KEY, PMC::make(b));

        //post the output tag downstream
        this->post_output_tag(0, t);
    }

    const size_t _mtu;
    boost::shared_ptr<asio::ip::udp::socket> socket;
    asio::ip::udp::endpoint *endpoint;
};

/***********************************************************************
 * Read input tags and send to a UDP socket.
 **********************************************************************/
struct UDPSocketSender : gras::Block
{
    UDPSocketSender(void):
        gras::Block("GrExtras UDPSocketSender")
    {
        //setup the input for messages only
        this->set_input_signature(gras::IOSignature(1));
        gras::InputPortConfig config = this->get_input_config(0);
        config.reserve_items = 0;
        this->set_input_config(0, config);
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        //iterate through all input tags, and post
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(0))
        {
            if (t.key == DATAGRAM_KEY and t.value.is<gras::SBuffer>())
            {
                const gras::SBuffer &b = t.value.as<gras::SBuffer>();
                socket->send_to(asio::buffer(b.get(), b.length), *endpoint);
            }
        }

        //erase all input tags from block
        this->erase_input_tags(0);

        //there should be no input items, consume all just in-case
        if (ins[0].size()) this->consume(0, ins[0].size());
    }

    boost::shared_ptr<asio::ip::udp::socket> socket;
    asio::ip::udp::endpoint *endpoint;
};

/***********************************************************************
 * UDPSocketMessage - hier block with sender and receiver
 **********************************************************************/

struct UDPSocketMessage : SocketMessage
{
    UDPSocketMessage(
        const std::string &type,
        const std::string &addr,
        const std::string &port,
        const size_t mtu
    ):
        gras::HierBlock("GrExtras UDPSocketMessage")
    {
        asio::ip::udp::resolver resolver(_io_service);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), addr, port);
        _endpoint = *resolver.resolve(query);

        _receiver = boost::make_shared<UDPSocketReceiver>(mtu);
        _sender = boost::make_shared<UDPSocketSender>();
        boost::shared_ptr<asio::ip::udp::socket> socket;

        if (type == "UDP_SERVER")
        {
            socket.reset(new asio::ip::udp::socket(_io_service, _endpoint));
            //TODO sender and receiver share endpoint - but no locking...

            _receiver->socket = socket;
            _receiver->endpoint = &_recv_ep;
            _sender->socket = socket;
            _sender->endpoint = &_recv_ep; //send to the last recv ep
        }

        if (type == "UDP_CLIENT")
        {
            socket.reset(new asio::ip::udp::socket(_io_service));
            socket->open(asio::ip::udp::v4());
            socket->connect(_endpoint);

            _receiver->socket = socket;
            _receiver->endpoint = &_recv_ep;
            _sender->socket = socket;
            _sender->endpoint = &_endpoint; //always send here
        }

        this->connect(*this, 0, _sender, 0);
        this->connect(_receiver, 0, *this, 0);

    }

    asio::io_service _io_service;
    asio::ip::udp::endpoint _endpoint, _recv_ep;
    boost::shared_ptr<UDPSocketReceiver> _receiver;
    boost::shared_ptr<UDPSocketSender> _sender;
};


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
    //TODO TCP
    throw std::runtime_error("SocketMessage::make fail - unknown type " + type);
}
