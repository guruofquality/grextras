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

#include <gnuradio/extras/socket_msg.h>
#include <gnuradio/block.h>
#include <gr_io_signature.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>
#include <iostream>

namespace asio = boost::asio;

using namespace gnuradio::extras;

static const long timeout_us = 100*1000; //100ms
static const pmt::pmt_t BLOB_KEY = pmt::pmt_string_to_symbol("blob_stream");
static const size_t POOL_SIZE = 64; //num pre-allocated blobs to acquire at once

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
 * produces messages to stream that recvd from socket
 **********************************************************************/
struct socket_msg_producer : gnuradio::block
{
    socket_msg_producer(const size_t mtu):
        gnuradio::block(
            "socket_msg_producer",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(size_t(0), size_t(1)) //1 output msg port
        ),
        _mtu(mtu)
    {
        std::stringstream str;
        str << name() << unique_id();
        _id = pmt::pmt_string_to_symbol(str.str());

        //pre-allocate blobs
        _mgr = pmt::pmt_mgr::make();
        for (size_t i = 0; i < POOL_SIZE; i++){
            _mgr->set(pmt::pmt_make_blob(mtu));
        }
    }

    int work(
        const InputItems &,
        const OutputItems &
    ){
        while (!boost::this_thread::interruption_requested())
        {
            //wait for socket to be init'd
            boost::shared_ptr<asio::ip::tcp::socket> _socket = socket;
            if (not _socket)
            {
                boost::this_thread::sleep(boost::posix_time::microseconds(timeout_us));
                continue;
            }

            if (!wait_for_recv_ready(_socket->native())) continue;

            //perform a blocking receive
            pmt::pmt_t blob = _mgr->acquire(true /*block*/);
            pmt::pmt_blob_resize(blob, _mtu);
            size_t num_bytes = 0;
            try{
                num_bytes = _socket->receive(asio::buffer(
                    pmt::pmt_blob_rw_data(blob), _mtu
                ));
            }catch(...){
                std::cerr << "socket msg block, socket receive error, continuing..." << std::endl;
                socket.reset();
            }

            //post the message to downstream subscribers
            pmt::pmt_blob_resize(blob, num_bytes);
            this->post_msg(0, BLOB_KEY, blob, _id);
        }
        return -1;
    }

    const size_t _mtu;
    pmt::pmt_t _id;
    pmt::pmt_mgr::sptr _mgr;

    boost::shared_ptr<asio::ip::tcp::socket> socket;
    boost::shared_ptr<asio::io_service> io_service;
};


/***********************************************************************
 * consumes messages from stream and send to socket
 **********************************************************************/
struct socket_msg_consumer : gnuradio::block
{
    socket_msg_consumer(void):
        gnuradio::block(
            "socket_msg_consumer",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(size_t(1), size_t(0)) //1 input msg port
        )
    {
        //NOP
    }

    int work(
        const InputItems &,
        const OutputItems &
    ){
        //loop for blobs until this thread is interrupted
        while (true){
            gr_tag_t msg = this->pop_msg_queue();
            if (!pmt::pmt_is_blob(msg.value)) continue;
            if (pmt::pmt_blob_length(msg.value) == 0) break; //empty blob, we are done here

            //if we dont have a socket, the message is dropped
            boost::shared_ptr<asio::ip::tcp::socket> _socket = socket;
            if (not _socket) continue;

            try{
                _socket->send(asio::buffer(
                    pmt::pmt_blob_data(msg.value),
                    pmt::pmt_blob_length(msg.value)
                ));
            }catch(...){
                std::cerr << "socket msg block, socket send error, continuing..." << std::endl;
                socket.reset();
            }
        }

        //when handle msgs finished, work is marked done
        return -1;
    }

    boost::shared_ptr<asio::ip::tcp::socket> socket;
    boost::shared_ptr<asio::io_service> io_service;
};

/***********************************************************************
 * Hier block that combines it all
 **********************************************************************/
class socket_msg_impl : public socket_msg{
public:
    socket_msg_impl(const std::string &addr, const std::string &port, const size_t mtu):
        gr_hier_block2(
            "socket_msg",
            gr_make_io_signature(1, 1, 1),
            gr_make_io_signature(1, 1, 1)
        )
    {
        //setup tcp listen service
        _io_service = boost::make_shared<asio::io_service>();
        asio::ip::tcp::resolver resolver(*_io_service);
        asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), addr, port);
        asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        _acceptor = boost::shared_ptr<asio::ip::tcp::acceptor>(new asio::ip::tcp::acceptor(*_io_service, endpoint));
        _tg.create_thread(boost::bind(&socket_msg_impl::serve, this));

        //make the blocks
        _consumer = gnuradio::get_initial_sptr(new socket_msg_consumer());
        _consumer->io_service = _io_service;
        _producer = gnuradio::get_initial_sptr(new socket_msg_producer(mtu));
        _producer->io_service = _io_service;

        //connect
        this->connect(this->self(), 0, _consumer, 0);
        this->connect(_producer, 0, this->self(), 0);
    }

    ~socket_msg_impl(void)
    {
        _tg.interrupt_all();
        _tg.join_all();
    }

private:

    void serve(void)
    {
        while (not boost::this_thread::interruption_requested())
        {
            if (!wait_for_recv_ready(_acceptor->native())) continue;
            boost::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(*_io_service));
            _acceptor->accept(*socket);

            //a synchronous switchover to a new client socket
            _consumer->socket = socket;
            _producer->socket = socket;
        }
    }

    boost::thread_group _tg;
    boost::shared_ptr<asio::io_service> _io_service;
    boost::shared_ptr<asio::ip::tcp::acceptor> _acceptor;

    boost::shared_ptr<socket_msg_consumer> _consumer;
    boost::shared_ptr<socket_msg_producer> _producer;
};

/***********************************************************************
 * Factory function
 **********************************************************************/
socket_msg::sptr socket_msg::make(const std::string &proto, const std::string &addr, const std::string &port, const size_t mtu_)
{
    const size_t mtu = (mtu_ == 0)? 1500 : mtu_;
    if (proto != "TCP") throw std::invalid_argument("unknown protocol for socket msg: " + proto);
    return gnuradio::get_initial_sptr(new socket_msg_impl(addr, port, mtu));
}
