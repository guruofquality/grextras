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

#include <gnuradio/extras/msg_passing.h>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/make_shared.hpp>
#include <queue>

using namespace gnuradio::extras;

/***********************************************************************
 * The message sinker implementation
 **********************************************************************/
class msg_sinker_impl : public msg_sinker
{
public:
    msg_sinker_impl(void):
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

boost::shared_ptr<msg_sinker> msg_sinker::make(void)
{
    return boost::make_shared<msg_sinker_impl>();
}

/***********************************************************************
 * The message sourcer implementation
 **********************************************************************/
class msg_sourcer_impl : public msg_sourcer
{
public:
    msg_sourcer_impl(void):
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

boost::shared_ptr<msg_sourcer> msg_sourcer::make(void)
{
    return boost::make_shared<msg_sourcer_impl>();
}
