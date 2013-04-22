// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/opencl_block.hpp>
%}

namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%include <gras/element.i>
%import <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/opencl_block.hpp>

%template(grextras_OpenClBlock) boost::shared_ptr<grextras::OpenClBlock>;

%pythoncode %{

OpenClBlock = OpenClBlock.make
__all__ = ["OpenClBlock"]

%}
