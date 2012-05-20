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

#include "block_gateway.h"
#include <gr_io_signature.h>
#include <iostream>
#include <boost/bind.hpp>

/***********************************************************************
 * The gr_block gateway implementation class
 **********************************************************************/
class block_gateway_impl : public block_gateway{
public:
    block_gateway_impl(
        gr_feval_ll *handler,
        const std::string &name,
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig,
        const gr_block_gw_work_type work_type,
        const unsigned factor,
        const gnuradio::extras::msg_signature &msg_sig
    ):
        block(name, in_sig, out_sig, msg_sig),
        _handler(handler),
        _work_type(work_type)
    {
        switch(_work_type){
        case GR_BLOCK_GW_WORK_GENERAL:
            this->set_sync(false);
            break;

        case GR_BLOCK_GW_WORK_SYNC:
            this->set_sync(true);
            break;

        case GR_BLOCK_GW_WORK_DECIM:
            this->set_sync(true);
            this->set_decim(factor);
            break;

        case GR_BLOCK_GW_WORK_INTERP:
            this->set_sync(true);
            this->set_interp(factor);
            this->set_output_multiple(factor);
            break;
        }
    }

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    void forecast(
        int noutput_items,
        gr_vector_int &ninput_items_required
    ){
        switch(_work_type){
        case GR_BLOCK_GW_WORK_GENERAL:
            _message.action = gr_block_gw_message_type::ACTION_FORECAST;
            _message.forecast_args_noutput_items = noutput_items;
            _message.forecast_args_ninput_items_required = ninput_items_required;
            _handler->calleval(0);
            ninput_items_required = _message.forecast_args_ninput_items_required;
            return;

        default:
            return gnuradio::extras::block::forecast(noutput_items, ninput_items_required);
        }
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        switch(_work_type){
        case GR_BLOCK_GW_WORK_GENERAL:
            _message.action = gr_block_gw_message_type::ACTION_GENERAL_WORK;
            break;
        default:
            _message.action = gr_block_gw_message_type::ACTION_WORK;
            break;
        }

        //FIXME do we need logic to do this?
        //if (!input_items.empty() && input_items[0].size() == 0) return -1;
        //if (!output_items.empty() && output_items[0].size() == 0) return -1;

        //setup the buffers
        _message.work_args_ninput_items.resize(input_items.size());
        _message.work_args_input_items.resize(input_items.size());
        for (size_t i = 0; i < input_items.size(); i++)
        {
            _message.work_args_ninput_items[i] = input_items[i].size();
            _message.work_args_input_items[i] = (void *)input_items[i].get();
        }

        _message.work_args_noutput_items.resize(output_items.size());
        _message.work_args_output_items.resize(output_items.size());
        for (size_t i = 0; i < output_items.size(); i++)
        {
            _message.work_args_noutput_items[i] = output_items[i].size();
            _message.work_args_output_items[i] = (void *)output_items[i].get();
        }

        //call the top level
        _handler->calleval(0);

        //and return result
        return _message.work_args_return_value;
    }

    bool start(void){
        _message.action = gr_block_gw_message_type::ACTION_START;
        _handler->calleval(0);
        return _message.start_args_return_value;
    }

    bool stop(void){
        _message.action = gr_block_gw_message_type::ACTION_STOP;
        _handler->calleval(0);
        return _message.stop_args_return_value;
    }

    gr_block_gw_message_type &gr_block_message(void){
        return _message;
    }

private:
    gr_feval_ll *_handler;
    gr_block_gw_message_type _message;
    const gr_block_gw_work_type _work_type;
};

block_gateway::sptr block_gateway::make(
    gr_feval_ll *handler,
    const std::string &name,
    gr_io_signature_sptr in_sig,
    gr_io_signature_sptr out_sig,
    const gr_block_gw_work_type work_type,
    const unsigned factor,
    const bool has_msg_input,
    const size_t num_msg_outputs
){
    return gnuradio::get_initial_sptr(
        new block_gateway_impl(handler, name, in_sig, out_sig, work_type, factor, gnuradio::extras::msg_signature(has_msg_input, num_msg_outputs))
    );
}
