// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_CLANG_BLOCK_HPP
#define INCLUDED_GREXTRAS_CLANG_BLOCK_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>
#include <string>

namespace grextras
{

/*!
 * The clang block parameters struct.
 * This struct contains all parameters necessary to compile a block.
 * The user fills this struct with source code and compiler options,
 * and then passes the filled-in struct to the ClangBlock::make routine.
 */
struct GREXTRAS_API ClangBlockParams
{
    ClangBlockParams(void);

    /*!
     * The name is the name of a factory function in the code.
     * The factory function has the following function prototype:
     *
     * gras::Block *example_factory_function(void);
     *
     * In this case, name would be "example_factory_function".
     */
    std::string name;

    /*!
     * The C++ source code.
     * The code contains the block definition and factory function.
     */
    std::string source;

    /*!
     * Flags are an optional list of compiler flags.
     * See the man page for clang for possible options.
     * Example: params.flags.push_back("-O3")
     */
    std::vector<std::string> flags;

    /*!
     * Include directories control the header file search path.
     * Users may leave this empty unless headers
     * are installed into non-standard directories.
     */
    std::vector<std::string> includes;
};

struct ClangBlock
{
    /*!
     * Create a Block from ClangBlockParams.
     * This routine will invoke clang and LLVM.
     * \param params the clang block parameters
     * \return a new Block created from JIT land
     */
    GREXTRAS_API static boost::shared_ptr<gras::Block>
        make(const ClangBlockParams &params);
};

}

#endif /*INCLUDED_GREXTRAS_CLANG_BLOCK_HPP*/
