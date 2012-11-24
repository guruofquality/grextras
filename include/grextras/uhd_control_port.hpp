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

#ifndef INCLUDED_GREXTRAS_UHD_CONTROL_PORT_HPP
#define INCLUDED_GREXTRAS_UHD_CONTROL_PORT_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The UHD control port block acts on asynchronous control messages.
 * This block has one input port that accepts messages.
 * TODO: message format
 */
struct GREXTRAS_API UHDControlPort : virtual gras::Block
{
    typedef boost::shared_ptr<UHDControlPort> sptr;

    /*!
     * Create a new UHD control port block.
     * The device address represents a device on your system.
     * To be portable, the addr is in the comma delimited format.
     * Ex: make("addr=192.168.10.2") 
     * http://files.ettus.com/uhd_docs/manual/html/identification.html
     */
    static sptr make(const std::string &addr);
};

}


#endif /*INCLUDED_GREXTRAS_UHD_CONTROL_PORT_HPP*/
