/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

//----------------------------------------------------------------------
//-- GR_EXTRAS_SWIG_BLOCK_FACTORY* convenience macros
//-- Usage: create a python wrapper for a block's factory function.
//-- A lot like gr_swig_block_magic.i, but
//--   * handles blocks in C++ namespaces
//--   * handles static make*() functions
//----------------------------------------------------------------------\

%define GR_EXTRAS_SWIG_BLOCK_FACTORY(NAME)
    GR_EXTRAS_SWIG_BLOCK_FACTORY_DECL(NAME)
    GR_EXTRAS_SWIG_BLOCK_FACTORY3(NAME, make, NAME)
%enddef

%define GR_EXTRAS_SWIG_BLOCK_FACTORY2(CLASS_NAME, SUFFIX)
    GR_EXTRAS_SWIG_BLOCK_FACTORY3(CLASS_NAME, make ## _ ## SUFFIX, CLASS_NAME ## _ ## SUFFIX)
%enddef

%define GR_EXTRAS_SWIG_BLOCK_FACTORY3(CLASS_NAME, FACTORY_NAME, PYTHON_NAME)
    %pythoncode %{
        PYTHON_NAME = CLASS_NAME.FACTORY_NAME
    %}
%enddef

%define GR_EXTRAS_SWIG_BLOCK_FACTORY_DECL(CLASS_NAME)
    %template(CLASS_NAME ## _sptr) boost::shared_ptr<CLASS_NAME>;
    %pythoncode %{
        CLASS_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id ())
    %}
%enddef
