// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/delay.hpp>
#include <grextras/time_align.hpp>
#include <grextras/burst_tagger.hpp>
#include <grextras/stream_selector.hpp>
#include <grextras/stream_to_datagram.hpp>
#include <grextras/datagram_to_stream.hpp>
#include <grextras/socket_message.hpp>
#include <grextras/tuntap.hpp>
#include <grextras/serialize_port.hpp>
#include <grextras/deserialize_port.hpp>
%}

//used in socket message, tuptap
%include <std_string.i>

//used by set_paths in stream selector
%include <std_vector.i>
%template (std_vector_int) std::vector<int>;

%include <grextras/delay.hpp>
%include <grextras/time_align.hpp>
%include <grextras/burst_tagger.hpp>
%include <grextras/stream_selector.hpp>
%include <grextras/stream_to_datagram.hpp>
%include <grextras/datagram_to_stream.hpp>
%include <grextras/socket_message.hpp>
%include <grextras/tuntap.hpp>
%include <grextras/serialize_port.hpp>
%include <grextras/deserialize_port.hpp>

GREXTRAS_SWIG_FOO(Delay)
GREXTRAS_SWIG_FOO(TimeAlign)
GREXTRAS_SWIG_FOO(BurstTagger)
GREXTRAS_SWIG_FOO(StreamSelector)
GREXTRAS_SWIG_FOO(Stream2Datagram)
GREXTRAS_SWIG_FOO(Datagram2Stream)
GREXTRAS_SWIG_FOO(SocketMessage)
GREXTRAS_SWIG_FOO(TunTap)
GREXTRAS_SWIG_FOO(SerializePort)
GREXTRAS_SWIG_FOO(DeserializePort)
