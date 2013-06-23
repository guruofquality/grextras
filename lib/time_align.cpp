// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/time_tag.hpp>
#include <grextras/time_align.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>
#include <iostream>

using namespace grextras;

typedef boost::int64_t int64_t;

struct TimeAlignImpl : TimeAlign
{
    TimeAlignImpl(const size_t itemsize):
        gras::Block("GrExtras TimeAlign"),
        _rate(1e6)
    {
        this->input_config(0).item_size = itemsize;
        this->output_config(0).item_size = itemsize;
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        _alignment_times.resize(num_inputs);
        _alignment_offsets.resize(num_inputs, 0);
    }

    void work(const InputItems &ins, const OutputItems &outs);

    //! get an absolute tick count for ins[i][0], where tick rate is the sample rate
    gras::item_index_t get_front_ticks(const size_t i)
    {
        gras::item_index_t ticks = _alignment_times[i].to_ticks(_rate);
        ticks += this->get_consumed(i) - _alignment_offsets[i];
        return ticks;
    }

    //store a time and the index it was found in a rx_time tag - per channel
    std::vector<gras::TimeTag> _alignment_times;
    std::vector<gras::item_index_t> _alignment_offsets;

    //cache the rate found in an rx_rate tag
    double _rate;
};

void TimeAlignImpl::work(const InputItems &ins, const OutputItems &outs)
{
    //search all inputs for time tags
    for (size_t i = 0; i < ins.size(); i++)
    {
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(i))
        {
            if (t.offset >= this->get_consumed(i) + ins[i].size()) continue;

            //extract rx time and rate values from tags if present
            try
            {
                const gras::StreamTag &st = t.object.as<gras::StreamTag>();
                if (st.key.as<std::string>() == "rx_rate")
                {
                    _rate = st.val.as<double>();
                }
                if (st.key.as<std::string>() == "rx_time")
                {
                    const gras::TimeTag new_time = gras::TimeTag::from_pmc(st.val);
                    _alignment_times[i] = new_time;
                    _alignment_offsets[i] = t.offset;
                }
            }
            catch(const std::invalid_argument &){continue;}
        }
    }

    //consume and dont forward inputs to force alignment
    size_t align_index = 0;
    gras::item_index_t align_ticks = this->get_front_ticks(align_index++);
    while (align_index < ins.size())
    {
        const gras::item_index_t front_ticks = this->get_front_ticks(align_index);

        //front ticks are equal, check next channel
        if (front_ticks == align_ticks) align_index++;

        //front ticks are newer, reset ticks, start loop again
        else if (front_ticks > align_ticks)
        {
            align_ticks = front_ticks;
            align_index = 0;
        }

        //front ticks are older, consume and return
        else if (front_ticks < align_ticks)
        {
            const size_t items = (align_ticks - front_ticks);
            this->consume(align_index, std::min(items, ins[align_index].size()));
            return; //we get called again ASAP if inputs are available
        }
    }

    //we are in alignment, forward all outputs
    for (size_t i = 0; i < ins.size(); i++)
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
