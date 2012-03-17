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
#include <gnuradio/blocks/add.h>
#include <gnuradio/blocks/add_const.h>
#include <gnuradio/blocks/divide.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/blocks/multiply_const.h>
#include <gnuradio/blocks/subtract.h>
%}

%include <gnuradio/blocks/add.h>
%include <gnuradio/blocks/add_const.h>
%include <gnuradio/blocks/divide.h>
%include <gnuradio/blocks/multiply.h>
%include <gnuradio/blocks/multiply_const.h>
%include <gnuradio/blocks/subtract.h>

////////////////////////////////////////////////////////////////////////
// template foo
////////////////////////////////////////////////////////////////////////
%template(set_const) gnuradio::blocks::add_const_v::set_const<std::complex<double> >;
%template(set_const) gnuradio::blocks::multiply_const_v::set_const<std::complex<double> >;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
using namespace gnuradio::blocks;

%define MAKE_ALL_THE_OP_TYPES(op)
    GR_SWIG_BLOCK_MAGIC2(op, fc32_fc32)
    GR_SWIG_BLOCK_MAGIC2(op, sc32_sc32)
    GR_SWIG_BLOCK_MAGIC2(op, sc16_sc16)
    GR_SWIG_BLOCK_MAGIC2(op, sc8_sc8)
    GR_SWIG_BLOCK_MAGIC2(op, f32_f32)
    GR_SWIG_BLOCK_MAGIC2(op, s32_s32)
    GR_SWIG_BLOCK_MAGIC2(op, s16_s16)
    GR_SWIG_BLOCK_MAGIC2(op, s8_s8)
%enddef

MAKE_ALL_THE_OP_TYPES(add)
MAKE_ALL_THE_OP_TYPES(multiply)
MAKE_ALL_THE_OP_TYPES(subtract)
MAKE_ALL_THE_OP_TYPES(divide)
MAKE_ALL_THE_OP_TYPES(add_const)
MAKE_ALL_THE_OP_TYPES(add_const_v)
MAKE_ALL_THE_OP_TYPES(multiply_const)
MAKE_ALL_THE_OP_TYPES(multiply_const_v)
