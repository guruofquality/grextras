// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/orc_block.hpp>
#include <grextras/opencl_block.hpp>
#include <grextras/clang_block.hpp>
%}

%include <std_string.i>
%include <std_vector.i>

%template (jit_blocks_vector_of_string) std::vector<std::string>;

%include <grextras/orc_block.hpp>
%include <grextras/opencl_block.hpp>
%include <grextras/clang_block.hpp>

GREXTRAS_SWIG_FOO(OrcBlock)
GREXTRAS_SWIG_FOO(OpenClBlock)

%template(gras_block) boost::shared_ptr<gras::Block>;

%pythoncode %{

#bring in the parameter structs as well
__all__.append("OpenClBlockParams")
__all__.append("OrcBlockParams")
__all__.append("ClangBlockParams")

#now add in the clang block make routine
ClangBlock = ClangBlock.make
__all__.append("ClangBlock")

%}
