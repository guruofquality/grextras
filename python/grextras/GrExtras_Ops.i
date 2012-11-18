// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/add.hpp>
#include <grextras/subtract.hpp>
#include <grextras/multiply.hpp>
#include <grextras/divide.hpp>
%}

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/add.hpp>
%include <grextras/subtract.hpp>
%include <grextras/multiply.hpp>
%include <grextras/divide.hpp>

%template(grextras_Add) boost::shared_ptr<grextras::Add>;
%template(grextras_Subtract) boost::shared_ptr<grextras::Subtract>;
%template(grextras_Multiply) boost::shared_ptr<grextras::Multiply>;
%template(grextras_Divide) boost::shared_ptr<grextras::Divide>;

%pythoncode %{

#remove the make_ from the factory functions, it feels more pythonic
for block in [Add, Subtract, Multiply, Divide]:
    for attr in filter(lambda x: x.startswith('make_'), dir(block)):
        make = getattr(block, attr)
        setattr(block, attr[5:], make)

__all__ = ["Add", "Subtract", "Multiply", "Divide"]

%}
