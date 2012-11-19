// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_STREAM_SELECTOR_HPP
#define INCLUDED_GREXTRAS_STREAM_SELECTOR_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>

namespace grextras
{

/*!
 * The stream selector block has N inputs and M outputs.
 * Any input port can be directed to any output port.
 * See the set_paths method for how this is controlled.
 * The implementation of this block is totally zero-copy.
 * Stream tags are Forwarded along with the item stream.
 */
struct GREXTRAS_API StreamSelector : virtual gras::Block
{
    typedef boost::shared_ptr<StreamSelector> sptr;

    /*!
     * Make a new stream selector block.
     * The user should initialize the IO signatures:
     * my_stream_selector->set_input_signature(...)
     * my_stream_selector->set_output_signature(...)
     */
    static sptr make(void);

    /*!
     * Set the path for samples for each input ports.
     *
     * Paths should be number of input ports in length.
     * Each element represents the destination for an input port.
     * The possible values for each element is an output port index,
     * or the value may also be: -1 to block or -2 to consume.
     *
     * Ex, a block with one input and two outputs:
     * paths = [1] input0 -> output1, output0 nothing.
     *
     * Ex, a block with two inputs and one output:
     * paths = [-1, 0] input0 blocks, input1 -> output0.
     *
     * \param paths a list of stream destinations for each port
     */
    virtual void set_paths(const std::vector<int> &paths) = 0;
};

}

#endif /*INCLUDED_GREXTRAS_STREAM_SELECTOR_HPP*/
