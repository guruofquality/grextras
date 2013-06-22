// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/time_tag.hpp>
#include <grextras/time_align.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>

using namespace grextras;

typedef boost::int64_t int64_t;

struct TimeAlignImpl : TimeAlign
{
    TimeAlignImpl(const size_t itemsize):
        gras::Block("GrExtras TimeAlign"),
        _alignment_state(false)
    {
        this->input_config(0).item_size = itemsize;
        this->output_config(0).item_size = itemsize;
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        _alignment_indexes.resize(num_inputs, 0);
    }

    void work(const InputItems &ins, const OutputItems &outs);

    gras::TimeTag _align_time;
    bool _alignment_state;
    std::vector<gras::item_index_t> _alignment_indexes;
};

void TimeAlignImpl::work(const InputItems &ins, const OutputItems &outs)
{
    const size_t n = ins.min();
    for (size_t i = 0; i < ins.size(); i++)
    {
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(i))
        {
            if (t.offset >= this->get_consumed(i) + n) continue;
            gras::TimeTag new_time;
            //extract a time tag of the expected format
            try
            {
                const gras::StreamTag &st = t.object.as<gras::StreamTag>();
                if (st.key.as<std::string>() != "rx_time") continue;
                new_time = gras::TimeTag::from_pmc(st.val);
            }
            catch(const std::invalid_argument &){continue;}

            //inspect the time tag for <, ==, >

            //equal
            if (new_time == _align_time)
            {
                _alignment_indexes[i] = t.offset;
            }

            //greater -- save this new time
            if (new_time > _align_time)
            {
                _alignment_indexes[i] = t.offset;
                _align_time = new_time;
            }

            //less -- this is an extinct time -- dump it
            if (new_time < _align_time)
            {
                this->consume(i, t.offset - this->get_consumed(i) + 1);
                return;
            }

        }
    }

    //TODO propagate tags in work here

    //we are in alignment, forward all outputs
    if (_alignment_state) for (size_t i = 0; i < ins.size(); i++)
    {
        //forward the entire input[i] to output[i]
        this->post_output_buffer(i, this->get_input_buffer(i));
        this->consume(i, ins[i].size());
    }
}

TimeAlign::sptr TimeAlign::make(const size_t itemsize)
{
    return boost::make_shared<TimeAlignImpl>(itemsize);
}
