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
        gras::Block("GrExtras TimeAlign")
    {
        this->input_config(0).item_size = itemsize;
        this->output_config(0).item_size = itemsize;
        this->set_sample_rate(1.0);
    }

    void set_sample_rate(const double rate)
    {
        _rate = rate;
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        _alignment_times.resize(num_inputs);
        _alignment_offsets.resize(num_inputs, 0);
    }

    void work(const InputItems &ins, const OutputItems &outs);

    gras::TimeTag get_front_time(const size_t i)
    {
        const gras::item_index_t delta = this->get_consumed(i) - _alignment_offsets[i];
        return _alignment_times[i] + gras::TimeTag::from_ticks(delta, _rate);
    }

    std::vector<gras::TimeTag> _alignment_times;
    std::vector<gras::item_index_t> _alignment_offsets;
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

            //extract a time tag of the expected format
            try
            {
                const gras::StreamTag &st = t.object.as<gras::StreamTag>();
                if (st.key.as<std::string>() != "rx_time") continue;
                const gras::TimeTag new_time = gras::TimeTag::from_pmc(st.val);
                _alignment_times[i] = new_time;
                _alignment_offsets[i] = t.offset;
            }
            catch(const std::invalid_argument &){continue;}
        }
    }

    //consume and dont forward inputs to force alignment
    size_t align_index = 0;
    gras::TimeTag align_time = this->get_front_time(align_index++);
    while (align_index < ins.size())
    {
        const gras::TimeTag front_time = this->get_front_time(align_index);

        //front time is equal, try next channel
        if (front_time == align_time) align_index++;

        //front is a newer time, reset time, start loop again
        else if (front_time > align_time)
        {
            align_time = front_time;
            align_index = 0;
        }

        //found an older time, consume and return
        else if (front_time < align_time)
        {
            const size_t items = (align_time - front_time).to_ticks(_rate);
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
