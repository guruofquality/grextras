// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/delay.hpp>
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

namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%include <gras/element.i>
%import <gras/block.i>
%import <gras/hier_block.i>
%include <grextras/config.hpp>
%include <grextras/delay.hpp>
%include <grextras/stream_selector.hpp>
%include <grextras/stream_to_datagram.hpp>
%include <grextras/datagram_to_stream.hpp>
%include <grextras/socket_message.hpp>
%include <grextras/tuntap.hpp>
%include <grextras/serialize_port.hpp>
%include <grextras/deserialize_port.hpp>

%template(grextras_Delay) boost::shared_ptr<grextras::Delay>;
%template(grextras_StreamSelector) boost::shared_ptr<grextras::StreamSelector>;
%template(grextras_Stream2Datagram) boost::shared_ptr<grextras::Stream2Datagram>;
%template(grextras_Datagram2Stream) boost::shared_ptr<grextras::Datagram2Stream>;
%template(grextras_SocketMessage) boost::shared_ptr<grextras::SocketMessage>;
%template(grextras_TunTap) boost::shared_ptr<grextras::TunTap>;
%template(grextras_SerializePort) boost::shared_ptr<grextras::SerializePort>;
%template(grextras_DeserializePort) boost::shared_ptr<grextras::DeserializePort>;

%pythoncode %{

Delay = Delay.make
StreamSelector = StreamSelector.make
Stream2Datagram = Stream2Datagram.make
Datagram2Stream = Datagram2Stream.make
SocketMessage = SocketMessage.make
TunTap = TunTap.make
SerializePort = SerializePort.make
DeserializePort = DeserializePort.make

__all__ = ["Delay", "StreamSelector", "Stream2Datagram", "Datagram2Stream", "SocketMessage", "TunTap", "SerializePort", "DeserializePort"]

%}
