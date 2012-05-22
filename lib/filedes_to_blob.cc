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

#include <gnuradio/extras/filedes_to_blob.h>
#include <gr_io_signature.h>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp> //select
#include <iostream>

#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <unistd.h>

using namespace gnuradio::extras;

static const pmt::pmt_t BLOB_KEY = pmt::pmt_string_to_symbol("blob_stream");
static const long timeout_us = 100*1000; //100ms
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

class filedes_to_blob_impl : public filedes_to_blob{
public:
    filedes_to_blob_impl(const int fd, const size_t mtu, const bool close):
        block(
            "filedes_to_blob",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0),
            msg_signature(false, 1)
        ),
        _fd(fd),
        _mtu(mtu),
        _close(close)
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

    ~filedes_to_blob_impl(void){
        if (_close) close(_fd);
    }

    int work(
        const InputItems &,
        const OutputItems &
    ){
        while (!boost::this_thread::interruption_requested()){
            if (!wait_for_recv_ready(_fd)) continue;

            //perform a blocking receive
            pmt::pmt_t blob = _mgr->acquire(true /*block*/);
            const int result = read(
                _fd, pmt::pmt_blob_rw_data(blob), _mtu
            );
            //std::cout << "read " << result << std::endl;
            if (result <= 0) std::cerr << "filedes_to_blob -> read error " << result << std::endl;

            //post the message to downstream subscribers
            pmt::pmt_blob_resize(blob, (result < 0)? 0 : size_t(result));
            this->post_msg(0, BLOB_KEY, blob, _id);
        }
        return -1;
    }

private:
    const int _fd;
    const size_t _mtu;
    const bool _close;
    pmt::pmt_t _id;
    pmt::pmt_mgr::sptr _mgr;
};

filedes_to_blob::sptr filedes_to_blob::make(
    const int fd, const size_t mtu_, const bool close_fd
){
    const size_t mtu = (mtu_ == 0)? 10000 : mtu_;
    return gnuradio::get_initial_sptr(new filedes_to_blob_impl(fd, mtu, close_fd));
}
