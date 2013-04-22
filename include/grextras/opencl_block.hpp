// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP
#define INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

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
};

}

#endif /*INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP*/
