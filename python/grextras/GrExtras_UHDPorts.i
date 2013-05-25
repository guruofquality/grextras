//
// Copyright 2012-2013 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

%include "GrExtras_Common.i"

%{
#include <grextras/uhd_control_port.hpp>
#include <grextras/uhd_status_port.hpp>
%}

%include <std_string.i>

%include <grextras/uhd_control_port.hpp>
%include <grextras/uhd_status_port.hpp>

GREXTRAS_SWIG_FOO(UHDControlPort)
GREXTRAS_SWIG_FOO(UHDStatusPort)
