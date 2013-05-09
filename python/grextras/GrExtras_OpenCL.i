// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/opencl_block.hpp>
%}

%include <std_string.i>
%include <std_vector.i>

%template (opencl_block_vector_of_string) std::vector<std::string>;

%include <grextras/config.hpp>
%include <grextras/opencl_block.hpp>

%template(grextras_OpenClBlock) boost::shared_ptr<grextras::OpenClBlock>;

%pythoncode %{

OpenClBlock = OpenClBlock.make
__all__ = ["OpenClBlock"]

%}
