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
// standard includes
////////////////////////////////////////////////////////////////////////
%include <gnuradio.i>
%include <gr_tags.i>

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gr_block_gateway.h>
%}

////////////////////////////////////////////////////////////////////////
// data type support
////////////////////////////////////////////////////////////////////////
%template(int_vector_t) std::vector<int>;
%template(void_star_vector_t) std::vector<void *>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_SWIG_BLOCK_MAGIC(gr,block_gateway);
%include <gr_block_gateway.h>

////////////////////////////////////////////////////////////////////////
// director stuff for handler
////////////////////////////////////////////////////////////////////////
%module(directors="1") gr_block_gw_swig;
%feature("director") gr_block_gw_handler_safe;
%feature("nodirector") gr_block_gw_handler_safe::call_handler;

%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

%{

// class that ensures we acquire and release the Python GIL
/* commented out because its defined somewhere else in core swig
class ensure_py_gil_state {
  PyGILState_STATE	d_gstate;
public:
  ensure_py_gil_state()  { d_gstate = PyGILState_Ensure(); }
  ~ensure_py_gil_state() { PyGILState_Release(d_gstate); }
};
*/
%}

%inline %{

class gr_block_gw_handler_safe : public gr_block_gw_handler{
public:
    void call_handle(void){
        ensure_py_gil_state _lock;
        return handle();
    }
};

%}

%inline %{

gr_tag_t gr_block_gw_pop_msg_queue_safe(boost::shared_ptr<gr_block_gateway> block_gw){
    gr_tag_t msg;
    GR_PYTHON_BLOCKING_CODE(
        msg = block_gw->gr_block__pop_msg_queue();
    )
    return msg;
}

%}
