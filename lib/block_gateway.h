/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRBLOCK_GATEWAY_H
#define INCLUDED_GRBLOCK_GATEWAY_H

#include <gnuradio/extras/api.h>
#include <gnuradio/block.h>
#include <stdexcept>
#include <gr_feval.h>

enum tag_propagation_policy_t {
    TPP_DONT = 0,
    TPP_ALL_TO_ALL = 1,
    TPP_ONE_TO_ONE = 2
};

/*!
 * Shared message structure between python and gateway.
 * Each action type represents a scheduler-called function.
 */
struct gr_block_gw_message_type{
    enum action_type{
        ACTION_WORK, //dispatch work
        ACTION_FORECAST, //dispatch forecast
        ACTION_START, //dispatch start
        ACTION_STOP, //dispatch stop
    };

    action_type action;

    std::vector<int> work_args_ninput_items;
    std::vector<int> work_args_noutput_items;
    std::vector<void *> work_args_input_items; //TODO this should be const void*, but swig cant int cast it right
    std::vector<void *> work_args_output_items;
    int work_args_return_value;

    int forecast_args_noutput_items;
    std::vector<int> forecast_args_ninput_items_required;

    bool start_args_return_value;

    bool stop_args_return_value;
};

/*!
 * The gateway block which performs all the magic.
 *
 * The gateway provides access to all the gr_block routines.
 * The methods prefixed with gr_block__ are renamed
 * to class methods without the prefix in python.
 */
class GR_EXTRAS_API block_gateway : virtual public gnuradio::block{
public:
    typedef boost::shared_ptr<block_gateway> sptr;

    /*!
     * Make a new gateway block.
     * \param handler the swig director object with callback
     * \param name the name of the block (Ex: "Shirley")
     * \param in_sig the input signature for this block
     * \param out_sig the output signature for this block
     * \return a new gateway block
     */
    static sptr make(
        gr_feval_ll *handler,
        const std::string &name,
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig,
        const size_t num_msg_inputs,
        const size_t num_msg_outputs
    );

    //! Provide access to the shared message object
    virtual gr_block_gw_message_type &gr_block_message(void) = 0;

    //! So the implementation knows to pass up forecast or call base
    virtual void set_auto_consume(const bool automatic) = 0;

    void gr_block__set_auto_consume(const bool automatic)
    {
        this->set_auto_consume(automatic);
        return gnuradio::block::set_auto_consume(automatic);
    }

    long gr_block__unique_id(void) const{
        return gnuradio::block::unique_id();
    }

    std::string gr_block__name(void) const{
        return gnuradio::block::name();
    }

    unsigned gr_block__history(void) const{
        return gnuradio::block::history();
    }

    void gr_block__set_history(unsigned history){
        return gnuradio::block::set_history(history);
    }

    void gr_block__set_output_multiple(int multiple){
        return gnuradio::block::set_output_multiple(multiple);
    }

    int gr_block__output_multiple(void) const{
        return gnuradio::block::output_multiple();
    }

    void gr_block__consume(int which_input, int how_many_items){
        return gnuradio::block::consume(which_input, how_many_items);
    }

    void gr_block__consume_each(int how_many_items){
        return gnuradio::block::consume_each(how_many_items);
    }

    void gr_block__produce(int which_output, int how_many_items){
        return gnuradio::block::produce(which_output, how_many_items);
    }

    void gr_block__set_relative_rate(double relative_rate){
        return gnuradio::block::set_relative_rate(relative_rate);
    }

    double gr_block__relative_rate(void) const{
        return gnuradio::block::relative_rate();
    }

    uint64_t gr_block__nitems_read(unsigned int which_input){
        return gnuradio::block::nitems_read(which_input);
    }

    uint64_t gr_block__nitems_written(unsigned int which_output){
        return gnuradio::block::nitems_written(which_output);
    }

    tag_propagation_policy_t gr_block__tag_propagation_policy(void){
        return (tag_propagation_policy_t)gnuradio::block::tag_propagation_policy();
    }

    void gr_block__set_tag_propagation_policy(tag_propagation_policy_t p){
        return gnuradio::block::set_tag_propagation_policy((gr_block::tag_propagation_policy_t)p);
    }

    void gr_block__add_item_tag(
        unsigned int which_output, const gr_tag_t &tag
    ){
        return gnuradio::block::add_item_tag(which_output, tag);
    }

    void gr_block__add_item_tag(
        unsigned int which_output,
        uint64_t abs_offset,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    ){
        return gnuradio::block::add_item_tag(which_output, abs_offset, key, value, srcid);
    }

    std::vector<gr_tag_t> gr_block__get_tags_in_range(
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end
    ){
        std::vector<gr_tag_t> tags;
        gnuradio::block::get_tags_in_range(tags, which_input, abs_start, abs_end);
        return tags;
    }

    std::vector<gr_tag_t> gr_block__get_tags_in_range(
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end,
        const pmt::pmt_t &key
    ){
        std::vector<gr_tag_t> tags;
        gnuradio::block::get_tags_in_range(tags, which_input, abs_start, abs_end, key);
        return tags;
    }

    bool gr_block__check_msg_queue(void){
        return gnuradio::block::check_msg_queue();
    }

    gr_tag_t gr_block__pop_msg_queue(void){
        return gnuradio::block::pop_msg_queue();
    }

    void gr_block__post_msg(const size_t port, const gr_tag_t &msg){
        return gnuradio::block::post_msg(port, msg);
    }

    void gr_block__post_msg(
        const size_t port,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    ){
        return gnuradio::block::post_msg(port, key, value, srcid);
    }

};

#endif /* INCLUDED_GRBLOCK_GATEWAY_H */
