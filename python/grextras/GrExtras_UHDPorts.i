//
// Copyright 2012 Ettus Research LLC
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

%{
#include <grextras/uhd_control_port.hpp>
#include <grextras/uhd_status_port.hpp>
%}

%include <std_string.i>

namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%include <gras/element.i>
%import <gras/block.i>
%include <grextras/config.hpp>
%include <grextras/uhd_control_port.hpp>
%include <grextras/uhd_status_port.hpp>

%template(grextras_UHDControlPort) boost::shared_ptr<grextras::UHDControlPort>;
%template(grextras_UHDStatusPort) boost::shared_ptr<grextras::UHDStatusPort>;

%pythoncode %{

UHDControlPort = UHDStatusPort.make
UHDStatusPort = UHDStatusPort.make

__all__ = ["UHDControlPort", "UHDStatusPort"]

%}
