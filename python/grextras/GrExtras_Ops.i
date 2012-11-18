// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/add.hpp>
%}

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/add.hpp>

%template(grextras_Add) boost::shared_ptr<grextras::Add>;

%pythoncode %{

for block in [Add]:
    for attr in filter(lambda x: x.startswith('make_'), dir(block)):
        make = getattr(block, attr)
        setattr(block, attr[5:], make)

__all__ = ["Add"]

%}
