// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/delay.hpp>
#include <boost/make_shared.hpp>

using namespace grextras;

struct DelayImpl : Delay
{
    DelayImpl(const size_t itemsize):
        gras::Block("GrExtras Delay")
    {
        //TODO set signature
        //this->
        _delay_items = 0;
    }

    void set_delay(const int num_items)
    {
        _delay_items = -num_items;
    }

    void work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        /*
        gruel::scoped_lock l(_delay_mutex);
        size_t noutput_items = output_items[0].size();
        const int delta = int64_t(nitems_read(0)) - int64_t(nitems_written(0)) - _delay_items;

        //consume but not produce (drops samples)
        if (delta < 0){
            this->consume_each(std::min(input_items[0].size(), size_t(-delta)));
            return 0;
        }

        //produce but not consume (inserts zeros)
        if (delta > 0){
            noutput_items = std::min(noutput_items, size_t(delta));
            std::memset(output_items[0].get(), 0, output_items[0].size()*_itemsize);
            return noutput_items;
        }

        //otherwise just memcpy
        noutput_items = std::min(noutput_items, input_items[0].size());
        std::memcpy(output_items[0].get(), input_items[0].get(), noutput_items*_itemsize);
        consume_each(noutput_items);
        return noutput_items;
        */
    }

    int _delay_items;
    //const size_t _itemsize;
};

Delay::sptr Delay::make(const size_t itemsize)
{
    return boost::make_shared<DelayImpl>(itemsize);
}
