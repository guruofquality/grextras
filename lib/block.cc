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

//gr implementation headers
#include <gr_block.h>
#include <gr_io_signature.h>

#include <gnuradio/extras/block.h>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/make_shared.hpp>
#include <queue>

using namespace gnuradio::extras;

/***********************************************************************
 * The message sourcer object
 **********************************************************************/
class msg_sourcer : public gr_block
{
public:
    msg_sourcer(void):
        gr_block(
            "msg_sourcer",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(1, 1, 1)
        )
    {
        //NOP
    }

    int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gr_tag_t msg;

        //loop for the msg from the producer
        boost::mutex::scoped_lock lock(_msg_queue_mutex);
        while (_msg_queue.empty())
        {
            _msg_queue_condition_variable.wait(lock);
            msg = _msg_queue.front();
            _msg_queue.pop();
            return 1;
        }

        //push the msg downstream
        msg.offset = this->nitems_written(0);
        this->add_item_tag(0, msg);

        //return produced
        return 1;
    }

    void post_msg(const gr_tag_t &msg)
    {
        boost::mutex::scoped_lock lock(_msg_queue_mutex);
        _msg_queue.push(msg);
        lock.unlock();
        _msg_queue_condition_variable.notify_one();
    }

private:
    std::vector<gr_tag_t> _tags;
    std::queue<gr_tag_t> _msg_queue;
    boost::mutex _msg_queue_mutex;
    boost::condition_variable _msg_queue_condition_variable;
};

/***********************************************************************
 * The message sinker object
 **********************************************************************/
class msg_sinker : public gr_block
{
public:
    msg_sinker(void):
        gr_block(
            "msg_sinker",
            gr_make_io_signature(1, 1, 1),
            gr_make_io_signature(0, 0, 0)
        )
    {
        //NOP
    }

    int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //read all tags associated with port 0 for items in this work function
        const uint64_t nread = this->nitems_read(0); //number of items read on port 0
        this->get_tags_in_range(_tags, 0, nread, nread+ninput_items[0]);
        this->consume(0, ninput_items[0]); //consume port 0 input

        //push the tags in the queue
        BOOST_FOREACH(const gr_tag_t &msg, _tags)
        {
            this->push_msg_queue(msg);
        }

        //return produced
        return 0;
    }

    void push_msg_queue(const gr_tag_t &msg){
        boost::mutex::scoped_lock lock(_msg_queue_mutex);
        _msg_queue.push(msg);
        lock.unlock();
        _msg_queue_condition_variable.notify_one();
    }

    bool check_msg_queue(void)
    {
        boost::mutex::scoped_lock lock(_msg_queue_mutex);
        return !_msg_queue.empty();
    }

    gr_tag_t pop_msg_queue(void)
    {
        boost::mutex::scoped_lock lock(_msg_queue_mutex);
        while (_msg_queue.empty())
        {
            _msg_queue_condition_variable.wait(lock);
            const gr_tag_t msg = _msg_queue.front();
            _msg_queue.pop();
            return msg;
        }
    }

private:
    std::vector<gr_tag_t> _tags;
    std::queue<gr_tag_t> _msg_queue;
    boost::mutex _msg_queue_mutex;
    boost::condition_variable _msg_queue_condition_variable;
};

/***********************************************************************
 * The master block object
 **********************************************************************/
class master_block : public gr_block
{
public:
    master_block(
        const std::string &name,
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig,
        block *parent
    ){
        _parent = parent;
        _input_items.resize(in_sig->max_streams());
        _output_items.resize(out_sig->max_streams());
    }

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    void forecast(
        int noutput_items,
        gr_vector_int &ninput_items_required
    ){
        if (!_sync)
        {
            return _parent->forecast(noutput_items, ninput_items_required);
        }

        else
        {
            unsigned ninputs = ninput_items_required.size();
            for (unsigned i = 0; i < ninputs; i++)
            {
                ninput_items_required[i] = fixed_rate_noutput_to_ninput(noutput_items);
            }
        }
    }

    int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //fill buffers
        for (size_t i = 0; i < input_items.size(); i++)
        {
            _input_items[i]._mem = input_items[i];
            _input_items[i]._len = ninput_items[i];
        }
        for (size_t i = 0; i < output_items.size(); i++)
        {
            _output_items[i]._mem = output_items[i];
            _output_items[i]._len = noutput_items;
        }

        //call work
        const int r = _parent->work(_input_items, _output_items);

        //consume when in sync
        if (!_sync && r > 0)
        {
            consume_each(r*_decim/_interp);
        }

        return r;
    }

    int fixed_rate_noutput_to_ninput(int noutput_items){
        return (noutput_items*_decim/_interp) + history() - 1;
    }

    int fixed_rate_ninput_to_noutput(int ninput_items){
        return std::max(0, ninput_items - (int)history() + 1)*_interp/_decim;
    }

    bool start(void){
        return _parent->start();
    }

    bool stop(void){
        return _parent->stop();
    }

    void set_sync(const bool sync)
    {
        _sync = sync;
        this->set_fixed_rate(sync);
    }

    void public_add_item_tag(
        unsigned int which_output, const gr_tag_t &tag
    ){
        return this->add_item_tag(which_output, tag);
    }

    void public_add_item_tag(
        unsigned int which_output,
        uint64_t abs_offset,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid
    ){
        return this->add_item_tag(which_output, abs_offset, key, value, srcid);
    }

    void public_get_tags_in_range(
        std::vector<gr_tag_t> &tags,
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end
    ){
        return this->get_tags_in_range(tags, which_input, abs_start, abs_end);
    }

    void public_get_tags_in_range(
        std::vector<gr_tag_t> &tags,
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end,
        const pmt::pmt_t &key
    ){
        return this->get_tags_in_range(tags, which_input, abs_start, abs_end, key);
    }

public: //public so block can set
    size_t _interp, _decim;

private:
    block *_parent;
    bool _sync;
    block::InputItems _input_items;
    block::OutputItems _output_items;
};

/***********************************************************************
 * The block object itself
 **********************************************************************/
//! The private guts of a block object
struct block::impl
{
    boost::shared_ptr<master_block> master;
    boost::shared_ptr<msg_sinker> sinker;
    std::vector<boost::shared_ptr<msg_sourcer> > sourcers;
};

static gr_io_signature_sptr extend_sig(gr_io_signature_sptr sig, const size_t num){
    std::vector<int> sizeof_stream_items = sig->sizeof_stream_items();
    for (size_t i = 0; i < num; i++)
    {
        sizeof_stream_items.push_back(1);
    }
    return gr_make_io_signaturev(sig->min_streams(), sig->max_streams(), sizeof_stream_items);
}

block::block(
    const std::string &name,
    gr_io_signature_sptr in_sig,
    gr_io_signature_sptr out_sig,
    const size_t num_msg_outs
):
    gr_hier_block2(
        name + " wrapper",
        extend_sig(in_sig, 1),
        extend_sig(out_sig, num_msg_outs)
    )
{
    _impl = boost::make_shared<impl>();
    _impl->master = boost::make_shared<master_block>(name, in_sig, out_sig, this);

    this->set_interp(1);
    this->set_decim(1);
    this->set_sync(false);

    //connect internal sink ports
    for (size_t i = 0; i < in_sig->max_streams(); i++)
    {
        this->connect(this->self(), i, _impl->master, i);
    }

    //connect internal source ports
    for (size_t i = 0; i < out_sig->max_streams(); i++)
    {
        this->connect(_impl->master, i, this->self(), i);
    }

    //connect sinker to upper port
    _impl->sinker = boost::make_shared<msg_sinker>();
    this->connect(this->self(), in_sig->max_streams(), _impl->sinker, 0);

    //connect sourcer to upper ports
    for (size_t i = 0; i < num_msg_outs; i++)
    {
        _impl->sourcers.push_back(boost::make_shared<msg_sourcer>());
        this->connect(_impl->sourcers.back(), i, this->self(), i+out_sig->max_streams());
    }
}

block::~block(void)
{
    //manual impl deconstruct, could be automatic through...
    _impl.reset();
}

void block::set_decim(const size_t decim)
{
    _impl->master->_decim = decim;
}

void block::set_interp(const size_t interp)
{
    this->set_output_multiple(interp);
    _impl->master->_interp = interp;
}

void block::set_sync(const bool sync)
{
    _impl->master->set_sync(sync);
}

/*******************************************************************
 * Basic routines from basic block
 ******************************************************************/

long block::unique_id(void) const
{
    return _impl->master->unique_id();
}

std::string block::name(void) const
{
    return _impl->master->name();
}

unsigned block::history(void) const
{
    return _impl->master->history();
}

void block::set_history(unsigned history)
{
    return _impl->master->set_history(history);
}

void block::set_output_multiple(int multiple)
{
    return _impl->master->set_output_multiple(multiple);
}

int block::output_multiple(void) const
{
    return _impl->master->output_multiple();
}

void block::consume(int which_input, int how_many_items)
{
    return _impl->master->consume(which_input, how_many_items);
}

void block::consume_each(int how_many_items)
{
    return _impl->master->consume_each(how_many_items);
}

void block::produce(int which_output, int how_many_items)
{
    return _impl->master->produce(which_output, how_many_items);
}

void block::set_relative_rate(double relative_rate)
{
    return _impl->master->set_relative_rate(relative_rate);
}

double block::relative_rate(void) const
{
    return _impl->master->relative_rate();
}

/*******************************************************************
 * Tag related routines from basic block
 ******************************************************************/

uint64_t block::nitems_read(unsigned int which_input)
{
    return _impl->master->nitems_read(which_input);
}

uint64_t block::nitems_written(unsigned int which_output)
{
    return _impl->master->nitems_written(which_output);
}

gr_block::tag_propagation_policy_t block::tag_propagation_policy(void)
{
    return _impl->master->tag_propagation_policy();
}

void block::set_tag_propagation_policy(gr_block::tag_propagation_policy_t p)
{
    return _impl->master->set_tag_propagation_policy(p);
}

void block::add_item_tag(
    unsigned int which_output, const gr_tag_t &tag
){
    return _impl->master->public_add_item_tag(which_output, tag);
}

void block::add_item_tag(
    unsigned int which_output,
    uint64_t abs_offset,
    const pmt::pmt_t &key,
    const pmt::pmt_t &value,
    const pmt::pmt_t &srcid
){
    return _impl->master->public_add_item_tag(which_output, abs_offset, key, value, srcid);
}

void block::get_tags_in_range(
    std::vector<gr_tag_t> &tags,
    unsigned int which_input,
    uint64_t abs_start,
    uint64_t abs_end
){
    return _impl->master->public_get_tags_in_range(tags, which_input, abs_start, abs_end);
}

void block::get_tags_in_range(
    std::vector<gr_tag_t> &tags,
    unsigned int which_input,
    uint64_t abs_start,
    uint64_t abs_end,
    const pmt::pmt_t &key
){
    return _impl->master->public_get_tags_in_range(tags, which_input, abs_start, abs_end, key);
}

/*******************************************************************
 * Message passing related routines from basic block
 ******************************************************************/

bool block::check_msg_queue(void)
{
    return _impl->sinker->check_msg_queue();
}

gr_tag_t block::pop_msg_queue(void)
{
    return _impl->sinker->pop_msg_queue();
}

void block::post_msg(const size_t &group, const gr_tag_t &msg)
{
    return _impl->sourcers.at(group)->post_msg(msg);
}

/*******************************************************************
 * Work related routines from basic block
 ******************************************************************/

bool block::start(void)
{
    return true;
}

bool block::stop(void)
{
    return true;
}

void block::forecast(int, gr_vector_int &)
{
    return;
}
