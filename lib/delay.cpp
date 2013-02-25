// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/delay.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

struct DelayImpl : Delay
{
    DelayImpl(const size_t itemsize):
        gras::Block("GrExtras Delay")
    {
        this->input_config(0).item_size = itemsize;
        this->output_config(0).item_size = itemsize;
        _delay_items = 0;
    }

    void set_delay(const int num_items)
    {
        //TODO not thread safe...
        _delay_items = -num_items;
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        size_t nouts = outs[0].size();
        const int delta = int64_t(this->get_consumed(0)) - int64_t(this->get_produced(0)) - _delay_items;

        //consume but not produce (drops samples)
        if (delta < 0)
        {
            this->consume(0, std::min(ins[0].size(), size_t(-delta)));
            return;
        }

        //produce but not consume (inserts zeros)
        if (delta > 0)
        {
            nouts = std::min(nouts, size_t(delta));
            std::memset(outs[0].get(), 0, outs[0].size()*this->output_config(0).item_size);
            this->produce(0, nouts);
            return;
        }

        //otherwise just forward the buffer
        gras::SBuffer buffer = this->get_input_buffer(0);
        this->post_output_buffer(0, buffer);
        this->consume(0, ins[0].size());
    }

    int _delay_items;
};

Delay::sptr Delay::make(const size_t itemsize)
{
    return boost::make_shared<DelayImpl>(itemsize);
}
