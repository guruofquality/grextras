// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <grextras/signal_source.hpp>
#include <grextras/noise_source.hpp>
%}

%include <std_complex.i>

%include "grextras_shared_ptr.i"
%include <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/signal_source.hpp>
%include <grextras/noise_source.hpp>

%template(grextras_SignalSource) boost::shared_ptr<grextras::SignalSource>;
%template(grextras_NoiseSource) boost::shared_ptr<grextras::NoiseSource>;

%pythoncode %{

SignalSource = SignalSource.make
NoiseSource = NoiseSource.make

__all__ = ["SignalSource", "NoiseSource"]

%}
