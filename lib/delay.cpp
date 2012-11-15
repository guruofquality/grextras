// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/delay.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

struct DelayImpl : Delay
{
    DelayImpl(const size_t itemsize):
        gras::Block("GrExtras Delay")
    {
        this->set_input_signature(gras::IOSignature(itemsize));
        this->set_output_signature(gras::IOSignature(itemsize));
        _delay_items = 0;
    }

    void set_delay(const int num_items)
    {
        //TODO not thread safe...
        _delay_items = -num_items;
    }

    void work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        size_t noutput_items = output_items[0].size();
        const int delta = int64_t(this->get_consumed(0)) - int64_t(this->get_produced(0)) - _delay_items;

        //consume but not produce (drops samples)
        if (delta < 0)
        {
            //TODO tags here
            this->consume(0, std::min(input_items[0].size(), size_t(-delta)));
            return;
        }

        //produce but not consume (inserts zeros)
        if (delta > 0)
        {
            noutput_items = std::min(noutput_items, size_t(delta));
            std::memset(output_items[0].get(), 0, output_items[0].size()*this->output_signature()[0]);
            this->produce(0, noutput_items);
            return;
        }

        //otherwise just forward the buffer
        //TODO tags here
        const gras::SBuffer &buffer = this->get_input_buffer(0);
        this->post_output_buffer(0, buffer);
        this->consume(0, input_items[0].size());
    }

    int _delay_items;
};

Delay::sptr Delay::make(const size_t itemsize)
{
    return boost::make_shared<DelayImpl>(itemsize);
}
