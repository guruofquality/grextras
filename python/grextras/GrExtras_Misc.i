// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/delay.hpp>
%}

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/delay.hpp>

%template(grextras_Delay) boost::shared_ptr<grextras::Delay>;

%pythoncode %{

Delay = Delay.make

__all__ = ["Delay"]

%}
