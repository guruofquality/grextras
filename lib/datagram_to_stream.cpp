// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/datagram_to_stream.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

struct Datagram2StreamImpl : Datagram2Stream
{
    Datagram2StreamImpl(const size_t itemsize):
        gras::Block("GrExtras Datagram2Stream")
    {
        //setup the input for messages only
        this->input_config(0).reserve_items = 0;

        //setup the output for streaming
        this->output_config(0).item_size = itemsize;
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        //read the input message, and post
        const PMCC msg = this->pop_input_msg(0);
        if (not msg.is<gras::PacketMsg>()) return;
        gras::SBuffer buff = msg.as<gras::PacketMsg>().buff;
        buff.length -= buff.length % this->output_config(0).item_size;
        this->post_output_buffer(0, buff);
    }
};

Datagram2Stream::sptr Datagram2Stream::make(const size_t itemsize)
{
    return boost::make_shared<Datagram2StreamImpl>(itemsize);
}
