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

////////////////////////////////////////////////////////////////////////
// standard includes
////////////////////////////////////////////////////////////////////////
%include <gruel_common.i>
%include <gnuradio.i>
%include <gr_tags.i>
%include <gr_feval.h>

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <block_gateway.h>
%}

%include <block_gateway.h>

////////////////////////////////////////////////////////////////////////
// data type support
////////////////////////////////////////////////////////////////////////
%template(int_vector_t) std::vector<int>;
%template(void_star_vector_t) std::vector<void *>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_EXTRAS_SWIG_BLOCK_FACTORY(block_gateway);

////////////////////////////////////////////////////////////////////////
// safe foo
////////////////////////////////////////////////////////////////////////
%inline %{

gr_tag_t gr_block_gw_pop_msg_queue_safe(boost::shared_ptr<block_gateway> block_gw){
    gr_tag_t msg;
    GR_PYTHON_BLOCKING_CODE(
        msg = block_gw->gr_block__pop_msg_queue();
    )
    return msg;
}

%}
