// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/orc_block.hpp>
%}

%include <std_string.i>
%include <std_vector.i>

%template (orc_block_vector_of_string) std::vector<std::string>;

%include <grextras/orc_block.hpp>

GREXTRAS_SWIG_FOO(OrcBlock)
