// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/datagram_to_stream.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

using namespace grextras;

static const PMCC DATAGRAM_KEY = PMC_M("datagram");

struct Datagram2StreamImpl : Datagram2Stream
{
    Datagram2StreamImpl(const size_t itemsize):
        gras::Block("GrExtras Datagram2Stream")
    {
        //setup the input for messages only
        this->set_input_signature(gras::IOSignature(1));
        gras::InputPortConfig config = this->get_input_config(0);
        config.reserve_items = 0;
        this->set_input_config(0, config);

        //setup the output for streaming
        this->set_output_signature(gras::IOSignature(itemsize));
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        //iterate through all input tags, and post
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(0))
        {
            if (t.key == DATAGRAM_KEY and t.value.is<gras::SBuffer>())
            {
                this->post_output_buffer(0, t.value.as<gras::SBuffer>());
            }
            else
            {
                this->post_output_tag(0, t); //not a buffer! post as inline tag
            }
        }

        //erase all input tags from block
        this->erase_input_tags(0);

        //there should be no input items, consume all just in-case
        if (ins[0].size()) this->consume(0, ins[0].size());
    }
};

Datagram2Stream::sptr Datagram2Stream::make(const size_t itemsize)
{
    return boost::make_shared<Datagram2StreamImpl>(itemsize);
}
