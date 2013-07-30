// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <PMC/Containers.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>

struct Datagram2Stream : gras::Block
{
    Datagram2Stream(const size_t itemsize):
        gras::Block("GrExtras Datagram2Stream")
    {
        //setup the output for streaming
        this->output_config(0).item_size = itemsize;
    }

    void work(const InputItems &, const OutputItems &)
    {
        //read the input message, and post
        const PMCC msg = this->pop_input_msg(0);
        if (not msg.is<gras::PacketMsg>()) return;
        const gras::PacketMsg &pkt_msg = msg.as<gras::PacketMsg>();

        //post any tags in the info
        if (pkt_msg.info)
        {
            try
            {
                BOOST_FOREACH(const PMCC &tag_p, pkt_msg.info.as<PMCList>())
                {
                    gras::Tag tag = tag_p.as<gras::Tag>();
                    tag.offset += this->get_produced(0);
                    this->post_output_tag(0, tag);
                }
            }
            catch(const std::invalid_argument &){}
        }

        //post the output buffer
        gras::SBuffer buff = pkt_msg.buff;
        buff.length -= buff.length % this->output_config(0).item_size;
        this->post_output_buffer(0, buff);
    }
};

GRAS_REGISTER_FACTORY1("/extras/datagram_to_stream", Datagram2Stream, size_t)
