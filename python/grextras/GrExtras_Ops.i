// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

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

%include <grextras/add.hpp>
%include <grextras/add_const.hpp>
%include <grextras/subtract.hpp>
%include <grextras/multiply.hpp>
%include <grextras/multiply_const.hpp>
%include <grextras/divide.hpp>

GREXTRAS_SWIG_FOO(Add)
GREXTRAS_SWIG_FOO(AddConst)
GREXTRAS_SWIG_FOO(AddConstV)
GREXTRAS_SWIG_FOO(Subtract)
GREXTRAS_SWIG_FOO(Multiply)
GREXTRAS_SWIG_FOO(MultiplyConst)
GREXTRAS_SWIG_FOO(MultiplyConstV)
GREXTRAS_SWIG_FOO(Divide)
