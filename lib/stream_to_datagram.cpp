// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/stream_to_datagram.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

struct Stream2DatagramImpl : Stream2Datagram
{
    Stream2DatagramImpl(const size_t itemsize, const size_t mtu):
        gras::Block("GrExtras Stream2Datagram"),
        _mtu(itemsize*(mtu/itemsize)) //ensure mtu is a multiple
    {
        //setup the input for streaming
        this->input_config(0).item_size = itemsize;
    }

    void work(const InputItems &, const OutputItems &)
    {
        //grab the input buffer on port 0
        gras::SBuffer b = this->get_input_buffer(0);

        //clip the buffer length to the mtu
        if (_mtu) b.length = std::min(b.length, _mtu);

        //create a message for this buffer
        const gras::PacketMsg msg(b);

        //post the output message downstream
        this->post_output_msg(0, PMC_M(msg));

        //consume the number of items in b
        this->consume(0, b.length/this->input_config(0).item_size);
    }

    const size_t _mtu;
};

Stream2Datagram::sptr Stream2Datagram::make(const size_t itemsize, const size_t mtu)
{
    return boost::make_shared<Stream2DatagramImpl>(itemsize, mtu);
}
