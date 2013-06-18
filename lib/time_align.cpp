// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/time_align.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/cstdint.hpp>
#include <PMC/Containers.hpp> //tuple
#include <stdexcept>

using namespace grextras;

typedef boost::int64_t int64_t;

struct TimeAlignImpl : TimeAlign
{
    TimeAlignImpl(const size_t itemsize):
        gras::Block("GrExtras TimeAlign"),
        _full_secs(0),
        _frac_secs(0),
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

    int64_t _full_secs;
    double _frac_secs;
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
            double full_secs;
            int64_t frac_secs;
            //extract a time tag of the expected format
            try
            {
                const gras::StreamTag &st = t.object.as<gras::StreamTag>();
                if (st.key.as<std::string>() != "rx_time") continue;
                const PMCTuple<2> &tuple = st.val.as<PMCTuple<2> >();
                full_secs = tuple[0].as<double>();
                frac_secs = tuple[1].as<boost::int64_t>();
            }
            catch(const std::invalid_argument &){continue;}

            //inspect the time tag for <, ==, >

            //equal
            if (full_secs == _full_secs and frac_secs == _frac_secs)
            {
                _alignment_indexes[i] = t.offset;
            }

            //greater -- save this new time
            if (full_secs > _full_secs or (full_secs == _full_secs and frac_secs > _frac_secs))
            {
                _alignment_indexes[i] = t.offset;
                _full_secs = full_secs;
                _frac_secs = frac_secs;
            }

            //less -- this is an extinct time -- dump it
            if (full_secs < _full_secs or (full_secs == _full_secs and frac_secs < _frac_secs))
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
