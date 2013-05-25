// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/signal_source.hpp>
#include <grextras/noise_source.hpp>
%}

%include <std_complex.i>

%include <grextras/signal_source.hpp>
%include <grextras/noise_source.hpp>

GREXTRAS_SWIG_FOO(SignalSource)
GREXTRAS_SWIG_FOO(NoiseSource)
