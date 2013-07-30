// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <boost/foreach.hpp>
#include <iostream>

struct BurstTagger : gras::Block
{
    BurstTagger(const size_t sps):
        gras::Block("GrExtras BurstTagger"),
        _sps(sps)
    {
        this->input_config(0).item_size = 8;
        this->output_config(0).item_size = 8;
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        const size_t n = ins[0].size();

        //find length tag and create an EOB
        BOOST_FOREACH(gras::Tag t, this->get_input_tags(0))
        {
            //filter out tags past the available input
            if (t.offset >= this->get_consumed(0) + n) continue;

            //filter out non length tags
            if (not t.object.is<gras::StreamTag>()) continue;
            PMCC key = t.object.as<gras::StreamTag>().key;
            PMCC val = t.object.as<gras::StreamTag>().val;
            if (not key.is<std::string>()) continue;
            if (key.as<std::string>() != "length") continue;
            if (not val.is<size_t>()) continue;

            //create EOB tag
            const size_t &length = val.as<size_t>();
            gras::StreamTag st(PMC_M("tx_eob"), PMC_M(true));
            gras::item_index_t offset = t.offset;
            offset -= this->get_consumed(0);
            offset += length*_sps - 1;
            offset += this->get_produced(0);
            this->post_output_tag(0, gras::Tag(offset, PMC_M(st)));
        }

        //forward the input[0] to output[0]
        gras::SBuffer buffer = this->get_input_buffer(0);
        this->post_output_buffer(0, buffer);
        this->consume(0, n);
    }

    const size_t _sps;
};

GRAS_REGISTER_FACTORY1("/extras/burst_tagger", BurstTagger, size_t)
