// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/add.hpp>
#include <grextras/add_const.hpp>
#include <grextras/subtract.hpp>
#include <grextras/multiply.hpp>
#include <grextras/multiply_const.hpp>
#include <grextras/divide.hpp>
%}

%include <stdint.i>
%include <std_complex.i>

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <gras/hier_block.hpp>
%include <grextras/config.hpp>
%include <grextras/add.hpp>
%include <grextras/add_const.hpp>
%include <grextras/subtract.hpp>
%include <grextras/multiply.hpp>
%include <grextras/multiply_const.hpp>
%include <grextras/divide.hpp>

%template(grextras_Add) boost::shared_ptr<grextras::Add>;
%template(grextras_AddConst) boost::shared_ptr<grextras::AddConst>;
%template(grextras_AddConstV) boost::shared_ptr<grextras::AddConstV>;
%template(grextras_Subtract) boost::shared_ptr<grextras::Subtract>;
%template(grextras_Multiply) boost::shared_ptr<grextras::Multiply>;
%template(grextras_MultiplyConst) boost::shared_ptr<grextras::MultiplyConst>;
%template(grextras_MultiplyConstV) boost::shared_ptr<grextras::MultiplyConstV>;
%template(grextras_Divide) boost::shared_ptr<grextras::Divide>;

%pythoncode %{

#remove the make_ from the factory functions, it feels more pythonic
for block in [Add, AddConst, AddConstV, Subtract, Multiply, MultiplyConst, MultiplyConstV, Divide]:
    for attr in filter(lambda x: x.startswith('make_'), dir(block)):
        make = getattr(block, attr)
        setattr(block, attr[5:], make)

__all__ = ["Add", "AddConst", "AddConstV", "Subtract", "Multiply", "MultiplyConst", "MultiplyConstV", "Divide"]

%}
