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
#include <gnuradio/extras/signal_source.h>
#include <gnuradio/extras/noise_source.h>
%}

%include <gnuradio/extras/noise_source.h>
%include <gnuradio/extras/signal_source.h>

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
using namespace gnuradio::extras;

%define MAKE_ALL_THE_SOURCE_TYPES(src)
    GR_EXTRAS_SWIG_BLOCK_FACTORY_DECL(src)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, fc32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, sc32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, sc16)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, sc8)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, f32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, s32)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, s16)
    GR_EXTRAS_SWIG_BLOCK_FACTORY2(src, s8)
%enddef

MAKE_ALL_THE_SOURCE_TYPES(noise_source)
MAKE_ALL_THE_SOURCE_TYPES(signal_source)
