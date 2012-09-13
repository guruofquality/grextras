/*
 * Copyright 2012 Free Software Foundation, Inc.
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
#include <gnuradio/extras/blob_to_filedes.h>
#include <gnuradio/extras/blob_to_socket.h>
#include <gnuradio/extras/blob_to_stream.h>
#include <gnuradio/extras/filedes_to_blob.h>
#include <gnuradio/extras/socket_to_blob.h>
#include <gnuradio/extras/stream_to_blob.h>
#include <gnuradio/extras/tuntap.h>
#include <gnuradio/extras/msg_many_to_one.h>
#include <gnuradio/extras/socket_msg.h>
%}

%include <gnuradio/extras/blob_to_filedes.h>
%include <gnuradio/extras/blob_to_socket.h>
%include <gnuradio/extras/blob_to_stream.h>
%include <gnuradio/extras/filedes_to_blob.h>
%include <gnuradio/extras/socket_to_blob.h>
%include <gnuradio/extras/stream_to_blob.h>
%include <gnuradio/extras/tuntap.h>
%include <gnuradio/extras/msg_many_to_one.h>
%include <gnuradio/extras/socket_msg.h>

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
using namespace gnuradio::extras;
GR_EXTRAS_SWIG_BLOCK_FACTORY(blob_to_filedes)
GR_EXTRAS_SWIG_BLOCK_FACTORY(blob_to_socket)
GR_EXTRAS_SWIG_BLOCK_FACTORY(blob_to_stream)
GR_EXTRAS_SWIG_BLOCK_FACTORY(filedes_to_blob)
GR_EXTRAS_SWIG_BLOCK_FACTORY(socket_to_blob)
GR_EXTRAS_SWIG_BLOCK_FACTORY(stream_to_blob)
GR_EXTRAS_SWIG_BLOCK_FACTORY(tuntap)
GR_EXTRAS_SWIG_BLOCK_FACTORY(msg_many_to_one)
GR_EXTRAS_SWIG_BLOCK_FACTORY(socket_msg)
