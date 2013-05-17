// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/orc_block.hpp>
%}

%include <std_string.i>
%include <std_vector.i>

%template (orc_block_vector_of_string) std::vector<std::string>;

%include <grextras/config.hpp>
%include <grextras/orc_block.hpp>

%template(grextras_OrcBlock) boost::shared_ptr<grextras::OrcBlock>;

%pythoncode %{

OrcBlock = OrcBlock.make
__all__ = ["OrcBlock"]

%}
