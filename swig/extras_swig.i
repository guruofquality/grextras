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


#define GR_EXTRAS_API

%ignore gr_hier_block2;

%include <extras_swig_doc.i>

////////////////////////////////////////////////////////////////////////
// standard includes
////////////////////////////////////////////////////////////////////////
%include <gnuradio.i>
%include "extras_factory.i"

//use a dummy block class to save on swig generation size
//%include <gnuradio/block.h>
namespace gnuradio {
    class block : public gr_hier_block2{};
}

namespace std {
    %template() vector< std::complex<double> >;
}

////////////////////////////////////////////////////////////////////////
// block includes
////////////////////////////////////////////////////////////////////////
%include <extras_ops.i>
%include <extras_delay.i>
%include <extras_sources.i>
%include <extras_stream_selector.i>
%include <extras_blobs.i>
%include <extras_block_gateway.i>
#ifdef HAVE_UHD
%include <extras_uhd_amsg_source.i>
#endif

////////////////////////////////////////////////////////////////////////
// this is not a block
////////////////////////////////////////////////////////////////////////
%include <extras_pmt.i>
