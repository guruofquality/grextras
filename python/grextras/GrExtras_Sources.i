// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/signal_source.hpp>
#include <grextras/noise_source.hpp>
%}

%include <std_complex.i>

namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%include <gras/element.i>
%import <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/signal_source.hpp>
%include <grextras/noise_source.hpp>

%template(grextras_SignalSource) boost::shared_ptr<grextras::SignalSource>;
%template(grextras_NoiseSource) boost::shared_ptr<grextras::NoiseSource>;

%pythoncode %{

#remove the make_ from the factory functions, it feels more pythonic
for block in [SignalSource, NoiseSource]:
    for attr in filter(lambda x: x.startswith('make_'), dir(block)):
        make = getattr(block, attr)
        setattr(block, attr[5:], make)

__all__ = ["SignalSource", "NoiseSource"]

%}
