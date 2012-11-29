// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/stream_to_datagram.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

static const PMCC DATAGRAM_KEY = PMC_M("datagram");

struct Stream2DatagramImpl : Stream2Datagram
{
    Stream2DatagramImpl(const size_t itemsize, const size_t mtu):
        gras::Block("GrExtras Stream2Datagram"),
        _mtu(itemsize*(mtu/itemsize)) //ensure mtu is a multiple
    {
        //setup the input for streaming
        this->set_input_signature(gras::IOSignature(itemsize));

        //setup the output for messages only
        this->set_output_signature(gras::IOSignature(1));
    }

    void work(const InputItems &, const OutputItems &)
    {
        //grab the input buffer on port 0
        gras::SBuffer b = this->get_input_buffer(0);

        //clip the buffer length to the mtu
        if (_mtu) b.length = std::min(b.length, _mtu);

        //create a tag for this buffer
        const gras::Tag t(0, DATAGRAM_KEY, PMC_M(b));

        //post the output tag downstream
        this->post_output_tag(0, t);

        //consume the number of items in b
        this->consume(0, b.length/this->input_signature()[0]);
    }

    const size_t _mtu;
};

Stream2Datagram::sptr Stream2Datagram::make(const size_t itemsize, const size_t mtu)
{
    return boost::make_shared<Stream2DatagramImpl>(itemsize, mtu);
}
