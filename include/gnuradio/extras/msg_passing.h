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

#ifndef INCLUDED_GR_EXTRAS_MSG_PASSING_H
#define INCLUDED_GR_EXTRAS_MSG_PASSING_H

#include <gnuradio/extras/api.h>
#include <gr_io_signature.h>
#include <gr_hier_block2.h>
#include <gr_block.h>
#include <gr_tags.h>

namespace gnuradio{ namespace extras{

//! Interface for a block with message passing
class EXTRAS_API msg_passing_base
{
protected:

    /*!
     * \brief Check if a message is available to pop.
     * \return true is a message is in the queue
     */
    virtual bool check_msg_queue(void) = 0;

    /*!
     * \brief Pop a message from the front of the queue.
     * This function will block until a message is available.
     * \return the message as a tag type
     */
    virtual gr_tag_t pop_msg_queue(void) = 0;

    /*!
     * \brief Post a message to a subscriber group.
     * All message subscribers in the group will get this message.
     * \param group the index of the subscriber group
     * \param msg the message to post to all subscribers
     */
    virtual void post_msg(const size_t &group, const gr_tag_t &msg) = 0;

    /*!
     * \brief Post a message to a subscriber group.
     *
     * \param group the index of the subscriber group
     * \param key the tag key as a PMT symbol
     * \param value any PMT holding any value for the given key
     * \param srcid optional source ID specifier; defaults to PMT_F
     */
    inline void post_msg(
        const size_t &group,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    ){
        gr_tag_t tag;
        tag.offset = 0; //not used
        tag.key = key;
        tag.value = value;
        tag.srcid = srcid;
        this->post_msg(group, tag);
    }
};

class 

//! Message sinker takes upstream messages
struct msg_sinker : public virtual gr_block{
    static boost::shared_ptr<msg_sinker> make(void);
    virtual bool check_msg_queue(void) = 0;
    virtual gr_tag_t pop_msg_queue(void) = 0;
};

//! Message sourcer produces messages to downstream
struct msg_sourcer : public virtual gr_block{
    static boost::shared_ptr<msg_sourcer> make(void);
    virtual void post_msg(const gr_tag_t &msg) = 0;
};

//!templated foo for making a block of any * that does messages
//! more docs TODO
template <typename BlockType>
class msg_passer : public gr_hier_block2, public virtual BlockType
{
public:
    msg_passer(
        const std::string &name,
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig,
        const size_t num_msg_sources
    ):
        gr_hier_block2(
            name,
            extend_sig(in_sig, 1),
            extend_sig(out_sig, num_msg_sources)
        ),
        BlockType(name, in_sig, out_sig)
    {
        //connect internal sink ports
        for (size_t i = 0; i < in_sig->max_streams(); i++)
        {
            this->connect(this->self(), i, BlockType::shared_from_this(), i);
        }

        //connect internal source ports
        for (size_t i = 0; i < out_sig->max_streams(); i++)
        {
            this->connect(BlockType::shared_from_this(), i, this->self(), i);
        }

        //connect sinker to upper port
        _sinker = msg_sinker::make();
        this->connect(this->self(), in_sig->max_streams(), _sinker, 0);

        //connect sourcer to upper ports
        for (size_t i = 0; i < num_msg_sources; i++)
        {
            _sourcers.push_back(msg_sourcer::make());
            this->connect(_sourcers.back(), i, this->self(), i+out_sig->max_streams());
        }
    }

    /*!
     * \brief Check if a message is available to pop.
     * \return true is a message is in the queue
     */
    bool check_msg_queue(void)
    {
        return _sinker->check_msg_queue();
    }

    /*!
     * \brief Pop a message from the front of the queue.
     * This function will block until a message is available.
     * \return the message as a tag type
     */
    gr_tag_t pop_msg_queue(void)
    {
        return _sinker->pop_msg_queue();
    }

    /*!
     * \brief Post a message to a subscriber group.
     * All message subscribers in the group will get this message.
     * \param group the index of the subscriber group
     * \param msg the message to post to all subscribers
     */
    void post_msg(const size_t &group, const gr_tag_t &msg){
        _sourcers.at(group)->post_msg(msg);
    }

    /*!
     * \brief Post a message to a subscriber group.
     *
     * \param group the index of the subscriber group
     * \param key the tag key as a PMT symbol
     * \param value any PMT holding any value for the given key
     * \param srcid optional source ID specifier; defaults to PMT_F
     */
    inline void post_msg(
        const size_t &group,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    ){
        gr_tag_t tag;
        tag.offset = 0; //not used
        tag.key = key;
        tag.value = value;
        tag.srcid = srcid;
        this->post_msg(group, tag);
    }

private:
    static gr_io_signature_sptr extend_sig(gr_io_signature_sptr sig, const size_t num){
        std::vector<int> sizeof_stream_items = sig->sizeof_stream_items();
        for (size_t i = 0; i < num; i++)
        {
            sizeof_stream_items.push_back(1);
        }
        return gr_make_io_signaturev(sig->min_streams(), sig->max_streams(), sizeof_stream_items);
    }

    boost::shared_ptr<msg_sinker> _sinker;
    std::vector<boost::shared_ptr<msg_sourcer> > _sourcers;
};

}}

#endif /* INCLUDED_GR_EXTRAS_MSG_PASSING_H */
