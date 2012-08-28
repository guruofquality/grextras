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

#include <gnuradio/extras/socket_to_blob.h>
#include <gr_io_signature.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

namespace asio = boost::asio;

using namespace gnuradio::extras;

static const long timeout_us = 100*1000; //100ms
static const pmt::pmt_t BLOB_KEY = pmt::pmt_string_to_symbol("blob_stream");
static const size_t POOL_SIZE = 64; //num pre-allocated blobs to acquire at once

static bool wait_for_recv_ready(int sock_fd){
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
 * UDP Implementation
 **********************************************************************/
class gr_udp_to_blob_impl : public socket_to_blob{
public:
    gr_udp_to_blob_impl(const std::string &addr, const std::string &port, const size_t mtu):
        block(
            "udp_to_blob",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(false, 1)
        ),
        _mtu(mtu)
    {
        asio::ip::udp::resolver resolver(_io_service);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), addr, port);
        asio::ip::udp::endpoint endpoint = *resolver.resolve(query);

        _socket = boost::shared_ptr<asio::ip::udp::socket>(new asio::ip::udp::socket(_io_service, endpoint));

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
        while (!boost::this_thread::interruption_requested()){
            if (!wait_for_recv_ready(_socket->native())) continue;

            //perform a blocking receive
            pmt::pmt_t blob = _mgr->acquire(true /*block*/);
            pmt::pmt_blob_resize(blob, _mtu);
            const size_t num_bytes = _socket->receive(asio::buffer(
                pmt::pmt_blob_rw_data(blob), _mtu
            ));

            //post the message to downstream subscribers
            pmt::pmt_blob_resize(blob, num_bytes);
            this->post_msg(0, BLOB_KEY, blob, _id);
        }
        return -1;
    }

private:
    asio::io_service _io_service;
    boost::shared_ptr<asio::ip::udp::socket> _socket;
    const size_t _mtu;
    pmt::pmt_t _id;
    pmt::pmt_mgr::sptr _mgr;
};

/***********************************************************************
 * TCP Implementation
 **********************************************************************/
class gr_tcp_to_blob_impl : public socket_to_blob{
public:
    gr_tcp_to_blob_impl(const std::string &addr, const std::string &port, const size_t mtu):
        block(
            "tcp_to_blob",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(false, 1)
        ),
        _mtu(mtu),
        _accepted(false)
    {
        asio::ip::tcp::resolver resolver(_io_service);
        asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), addr, port);
        asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

        _acceptor = boost::shared_ptr<asio::ip::tcp::acceptor>(new asio::ip::tcp::acceptor(_io_service, endpoint));
        _socket = boost::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(_io_service));

        std::stringstream str;
        str << name() << unique_id();
        _id = pmt::pmt_string_to_symbol(str.str());

        //pre-allocate blobs
        _mgr = pmt::pmt_mgr::make();
        for (size_t i = 0; i < POOL_SIZE; i++){
            _mgr->set(pmt::pmt_make_blob(mtu));
        }
    }

    /*!
     * Loop until the tcp acceptor gets a connection.
     * Note the interruption_point, so the scheduler can stop.
     */
    void accept(void){
        while (!_accepted){
            boost::this_thread::interruption_point();
            if (!wait_for_recv_ready(_acceptor->native())) continue;
            _acceptor->accept(*_socket);
            _accepted = true;
        }
    }

    int work(
        const InputItems &,
        const OutputItems &
    ){
        while (!boost::this_thread::interruption_requested()){
            if (!_accepted) this->accept();
            if (!wait_for_recv_ready(_socket->native())) continue;

            //perform a blocking receive
            pmt::pmt_t blob = _mgr->acquire(true /*block*/);
            pmt::pmt_blob_resize(blob, _mtu);
            const size_t num_bytes = _socket->receive(asio::buffer(
                pmt::pmt_blob_rw_data(blob), _mtu
            ));

            //post the message to downstream subscribers
            pmt::pmt_blob_resize(blob, num_bytes);
            this->post_msg(0, BLOB_KEY, blob, _id);
        }
        return -1;
    }

private:
    asio::io_service _io_service;
    boost::shared_ptr<asio::ip::tcp::socket> _socket;
    boost::shared_ptr<asio::ip::tcp::acceptor> _acceptor;
    const size_t _mtu;
    pmt::pmt_t _id;
    pmt::pmt_mgr::sptr _mgr;
    bool _accepted;
};

/***********************************************************************
 * Factory function
 **********************************************************************/
socket_to_blob::sptr socket_to_blob::make(
    const std::string &proto, const std::string &addr, const std::string &port, const size_t mtu_
){
    const size_t mtu = (mtu_ == 0)? 1500 : mtu_;
    if (proto == "UDP") return gnuradio::get_initial_sptr(new gr_udp_to_blob_impl(addr, port, mtu));
    if (proto == "TCP") return gnuradio::get_initial_sptr(new gr_tcp_to_blob_impl(addr, port, mtu));
    throw std::invalid_argument("unknown protocol for socket to blob: " + proto);
}
