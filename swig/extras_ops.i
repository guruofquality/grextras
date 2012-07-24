/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gnuradio/extras/add.h>
#include <gnuradio/extras/add_const.h>
#include <gnuradio/extras/divide.h>
#include <gnuradio/extras/multiply.h>
#include <gnuradio/extras/multiply_const.h>
#include <gnuradio/extras/subtract.h>
%}

%include <gnuradio/extras/add.h>
%include <gnuradio/extras/add_const.h>
%include <gnuradio/extras/divide.h>
%include <gnuradio/extras/multiply.h>
%include <gnuradio/extras/multiply_const.h>
%include <gnuradio/extras/subtract.h>

////////////////////////////////////////////////////////////////////////
// template foo
////////////////////////////////////////////////////////////////////////
%template(set_const) gnuradio::extras::add_const_v::set_const<std::complex<double> >;
%template(set_const) gnuradio::extras::multiply_const_v::set_const<std::complex<double> >;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
using namespace gnuradio::extras;

%define MAKE_ALL_THE_OP_TYPES(op)
    GR_EXTRAS_SWIG_BLOCK_FACTORY_DECL(op)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, fc32_fc32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, sc32_sc32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, sc16_sc16)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, sc8_sc8)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, f32_f32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, s32_s32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, s16_s16)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(op, s8_s8)
%enddef

MAKE_ALL_THE_OP_TYPES(add)
MAKE_ALL_THE_OP_TYPES(multiply)
MAKE_ALL_THE_OP_TYPES(subtract)
MAKE_ALL_THE_OP_TYPES(divide)
MAKE_ALL_THE_OP_TYPES(add_const)
MAKE_ALL_THE_OP_TYPES(add_const_v)
MAKE_ALL_THE_OP_TYPES(multiply_const)
MAKE_ALL_THE_OP_TYPES(multiply_const_v)
