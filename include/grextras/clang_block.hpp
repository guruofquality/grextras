// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_CLANG_BLOCK_HPP
#define INCLUDED_GREXTRAS_CLANG_BLOCK_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <vector>
#include <string>

namespace grextras
{

struct GREXTRAS_API ClangBlockParams
{
    ClangBlockParams(void);
    std::string name;
    std::string code;
    std::vector<std::string> flags;
    std::vector<std::string> include_dirs;

};

struct ClangBlock
{

    GREXTRAS_API static boost::shared_ptr<gras::Block>
    make(const ClangBlockParams &params);

};

}

#endif /*INCLUDED_GREXTRAS_CLANG_BLOCK_HPP*/
