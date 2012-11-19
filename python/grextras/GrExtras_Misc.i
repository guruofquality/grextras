// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/delay.hpp>
#include <grextras/stream_selector.hpp>
%}

//used by set_paths in stream selector
%include <std_vector.i>
%template (std_vector_int) std::vector<int>;

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/delay.hpp>
%include <grextras/stream_selector.hpp>

%template(grextras_Delay) boost::shared_ptr<grextras::Delay>;
%template(grextras_StreamSelector) boost::shared_ptr<grextras::StreamSelector>;

%pythoncode %{

Delay = Delay.make
StreamSelector = StreamSelector.make

__all__ = ["Delay", "StreamSelector"]

%}
