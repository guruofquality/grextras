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
     * Set the access mode for host memory in this block.
     * The parameter is a vector of strings; each string represents
     * the host buffer access mode for a port of the same index.
     * The parameter will be stretched from the back of the vector.
     * Therefore, homogeneous configurations require only one element.
     *
     * Possible options for an access mode:
     *  - "RW" = host reads and writes (default)
     *  - "RO" = host reads only
     *  - "WO" = host writes only
     *  - "XX" = no host access (pass device memory between blocks)
     *
     * \param direction "INPUT" or "OUTPUT" for port direction
     * \param modes the the vector of access modes
     */
    virtual void set_host_access_mode(const std::string &direction, const std::vector<std::string> &modes) = 0;

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
    virtual void set_device_access_mode(const std::string &direction, const std::vector<std::string> &modes) = 0;
};

}

#endif /*INCLUDED_GREXTRAS_OPENCL_BLOCK_HPP*/
