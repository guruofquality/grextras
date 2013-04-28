// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP
#define INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>
#include <string>

namespace grextras
{

/*!
 * Parameters to control production/consumption and kernel execution.
 */
struct GREXTRAS_API OpenClBlockParams
{
    OpenClBlockParams(void);

    /*!
     * The global size factor - a kenel execution parameter.
     * This factor controls how get_global_size() is calculated
     * based on the minimum number of items at each input port.
     * The global size value obeys the following calculation:
     *
     * get_global_size() = num_input_items*global_size_factor
     *
     * Default is 1.0
     */
    double global_factor;

    /*!
     * Local size - a kernel execution parameter.
     * Local size controls the local work size dimension.
     * The kernel will be called with this many work groups:
     * get_global_size()/get_local_size()
     *
     * Default is 1
     */
    size_t local_size;

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
struct GREXTRAS_API OpenClBlock : virtual gras::Block
{
    typedef boost::shared_ptr<OpenClBlock> sptr;

    /*!
     * Make a new OpenClBlock.
     * \param dev_type device type "CPU" or "CPU"
     * \return a new shared ptr for the block
     */
    static sptr make(const std::string &dev_type = "GPU");

    /*!
     * Get access to the special block parameters.
     */
    virtual OpenClBlockParams &params(void) = 0;

    /*!
     * Set the source code to be run by this block.
     * The source code is a cl program as a string.
     * Documentation for the possible options:
     * http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
     *
     * \param name name of a kernel in the source
     * \param source the cl source code to compile
     * \param options optional options, see docs
     */
    virtual void set_program(
        const std::string &name,
        const std::string &source,
        const std::string &options = ""
    ) = 0;

    /*!
     * Set the access mode for device memory in this block.
     * The parameter is a vector of strings; each string represents
     * the host buffer access mode for a port of the same index.
     * The parameter will be stretched from the back of the vector.
     * Therefore, homogeneous configurations require only one element.
     *
     * Possible options for an access mode:
     *  - "RW" = device reads and writes
     *  - "RO" = device reads only (default for input)
     *  - "WO" = device writes only (default for output)
     *
     * \param direction "INPUT" or "OUTPUT" for port direction
     * \param modes the the vector of access modes
     */
    virtual void set_access_mode(const std::string &direction, const std::vector<std::string> &modes) = 0;
};

}

#endif /*INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP*/
