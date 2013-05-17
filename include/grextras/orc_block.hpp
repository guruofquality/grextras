// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_ORC_BLOCK_HPP
#define INCLUDED_GREXTRAS_ORC_BLOCK_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>
#include <string>

namespace grextras
{

/*!
 * Parameters to control production/consumption and kernel execution.
 */
struct GREXTRAS_API OrcBlockParams
{
    OrcBlockParams(void);

    /*!
     * The ORC loop factor - a kenel execution parameter.
     * This factor controls the size of the ORC kernel loop
     * based on the minimum number of items at each input port.
     * The ORC loop size value obeys the following calculation:
     *
     * orc_work_size = num_input_items*orc_loop_factor
     *
     * Default is 1.0
     */
    double orc_loop_factor;

    /*!
     * The production factor - a production/consumption parameter.
     * This factor controls how items get produced from this block
     * based on the minimum number of items at each input port.
     * The production value obeys the following calculation:
     *
     * items_produced = num_input_items*production_factor
     *
     * Default is 1.0
     */
    double production_factor;

    /*!
     * The consumption offset - a production/consumption parameter.
     * This value controls how items get consumed from this block
     * based on the minimum number of items at each input port.
     * The consumption value obeys the following calculation:
     *
     * items_consumed = num_input_items-consumption_offset
     *
     * Default is 0
     */
    size_t consumption_offset;
};

/*!
 * The OpenCL Block creates a wrapper for using Open CL
 * within the GRAS buffering framework for maximum efficiency.
 * GRAS buffers are flexible, memory can be allocated such that
 * it can be used effectivly with upstream and downstream blocks.
 */
struct GREXTRAS_API OrcBlock : virtual gras::Block
{
    typedef boost::shared_ptr<OrcBlock> sptr;

    /*!
     * Make a new OrcBlock.
     * \return a new shared ptr for the block
     */
    static sptr make(void);

    /*!
     * Get access to the special block parameters.
     */
    virtual OrcBlockParams &params(void) = 0;

    /*!
     * Set the source code to be run by this block.
     * The source code is a ORC program as a string.
     *
     * \param name name of a kernel in the source
     * \param source the ORC source code to compile
     */
    virtual void set_program(
        const std::string &name,
        const std::string &source
    ) = 0;
};

}

#endif /*INCLUDED_GREXTRAS_ORC_BLOCK_HPP*/
