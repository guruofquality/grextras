/*
 * Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/extras/msg_many_to_one.h>
#include <gnuradio/block.h>
#include <gruel/thread.h>
#include <gr_io_signature.h>
#include <boost/foreach.hpp>
#include <queue>

using namespace gnuradio::extras;

class m21_output : public gnuradio::block
{
public:
    m21_output(void):
        gnuradio::block("m21 output", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, 1))
    {}

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        gruel::scoped_lock lock(mutex);
        while (queue.empty())
        {
            cond.wait(lock);
        }
        gr_tag_t tag = queue.front();
        queue.pop();
        tag.offset = this->nitems_written(0);
        this->add_item_tag(0, tag);
        return output_items[0].size();
    }

    std::queue<gr_tag_t> queue;
    gruel::mutex mutex;
    gruel::condition_variable cond;
};

class m21_input : public gnuradio::block
{
public:
    m21_input(void):
        gnuradio::block("m21 input", gr_make_io_signature(1, 1, 1), gr_make_io_signature(0, 0, 0))
    {}

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        const uint64_t nread = this->nitems_read(0);
        this->get_tags_in_range(tags, 0, nread, nread+input_items[0].size());
        BOOST_FOREACH(const gr_tag_t &tag, tags)
        {
            gruel::scoped_lock lock(output->mutex);
            output->queue.push(tag);
            output->cond.notify_one();
        }
        return input_items[0].size();
    }

    boost::shared_ptr<m21_output> output;
    std::vector<gr_tag_t> tags;
};

class msg_many_to_one_impl : public msg_many_to_one
{
public:
    msg_many_to_one_impl(const size_t num_inputs):
        gr_hier_block2(
            "message many to one",
            gr_make_io_signature(num_inputs, num_inputs, 1),
            gr_make_io_signature(1, 1, 1)
        )
    {
        output = gnuradio::get_initial_sptr(new m21_output());
        this->connect(output, 0, this->self(), 0);
        for (size_t i = 0; i < num_inputs; i++)
        {
            inputs.push_back(gnuradio::get_initial_sptr(new m21_input()));
            inputs[i]->output = output;
            this->connect(this->self(), i, inputs[i], 0);
        }
    }

    ~msg_many_to_one_impl(void)
    {
        inputs.clear();
        output.reset();
    }

    std::vector<boost::shared_ptr<m21_input> > inputs;
    boost::shared_ptr<m21_output> output;
};

msg_many_to_one::sptr msg_many_to_one::make(const size_t num_inputs)
{
    return gnuradio::get_initial_sptr(new msg_many_to_one_impl(num_inputs));
}
