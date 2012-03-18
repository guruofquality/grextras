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
//-- GR_SWIG_BLOCK_MAGIC1 and GR_SWIG_BLOCK_MAGIC2
//----------------------------------------------------------------------
%define GR_SWIG_BLOCK_MAGIC1(NAME)
_GR_SWIG_BLOCK_MAGIC2_HELPER(NAME, make, NAME)
%enddef

%define GR_SWIG_BLOCK_MAGIC2(CLASS_NAME, SUFFIX)
_GR_SWIG_BLOCK_MAGIC2_HELPER(CLASS_NAME, make ## _ ## SUFFIX, CLASS_NAME ## _ ## SUFFIX)
%enddef

#ifdef SWIGPYTHON
%define _GR_SWIG_BLOCK_MAGIC2_HELPER(CLASS_NAME, FACTORY_NAME, PYTHON_NAME)
%template(PYTHON_NAME ## _sptr) boost::shared_ptr<CLASS_NAME>;
%pythoncode %{
PYTHON_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id ())
PYTHON_NAME = CLASS_NAME.FACTORY_NAME
%}
%enddef
#endif

#define GR_EXTRAS_API

%ignore gr_block;
%ignore gr_sync_block;
%ignore gr_hier_block2;
%ignore gr_sync_decimator;

////////////////////////////////////////////////////////////////////////
// standard includes
////////////////////////////////////////////////////////////////////////
%include <gnuradio.i>

////////////////////////////////////////////////////////////////////////
// block includes
////////////////////////////////////////////////////////////////////////
%include <extras_ops.i>
%include <extras_delay.i>
%include <extras_sources.i>
%include <extras_stream_selector.i>
%include <extras_fir.i>
#ifdef HAVE_PMT_EXTENSIONS_AND_MSG_PASSING
%include <extras_blobs.i>
#endif
%include <extras_block_gateway.i>
#ifdef HAVE_UHD_AND_MSG_PASSING
%include <extras_uhd_amsg_source.i>
#endif
