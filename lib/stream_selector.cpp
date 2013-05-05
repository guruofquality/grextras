// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/stream_selector.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

using namespace grextras;

struct StreamSelectorImpl : StreamSelector
{
    StreamSelectorImpl(const size_t itemsize):
        gras::Block("GrExtras StreamSelector")
    {
        this->input_config(0).item_size = itemsize;
        this->output_config(0).item_size = itemsize;
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        for (size_t i = 0; i < ins.size(); i++)
        {
            if (ins[i].size() == 0) continue;
            switch(_paths[i])
            {
            case -2: //consume, no forward
                this->consume(i, ins[i].size());
                break;

            case -1: //block, dont consume
                break;

            default: //forward the buffer
                gras::SBuffer buffer = this->get_input_buffer(i);
                this->post_output_buffer(_paths[i], buffer);
                this->consume(i, ins[i].size());
                break;
            }
        }
    }

    void propagate_tags(const size_t i, const gras::TagIter &iter)
    {
        if (_paths[i] < 0) return; //bye tags!
        BOOST_FOREACH(gras::Tag t, iter)
        {
            //forward input tags to selected outputs
            t.offset -= this->get_consumed(i);
            t.offset += this->get_produced(_paths[i]);
            this->post_output_tag(_paths[i], t);
        }
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        _paths.resize(num_inputs, -1);
        for (size_t i = 0; i < num_inputs; i++)
        {
            //set the reserve_items to zero:
            //work gets called when not all inputs are fed
            this->input_config(i).reserve_items = 0;

            //only done when all inputs are done
            this->input_config(i).force_done = false;
        }
    }

    void set_paths(const std::vector<int> &paths)
    {
        _paths = paths;
    }

    std::vector<int> _paths;
};

StreamSelector::sptr StreamSelector::make(const size_t itemsize)
{
    return boost::make_shared<StreamSelectorImpl>(itemsize);
}
