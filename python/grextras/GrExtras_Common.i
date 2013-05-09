// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <gras/block.hpp>
#include <gras/hier_block.hpp>
#include <gras/top_block.hpp>
%}

%include <gras/exception.i>
%include <gras/element.i>
%import <gras/block.i>
%import <gras/hier_block.i>

namespace boost{template<class T>struct shared_ptr{T*operator->();};}
