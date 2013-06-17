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

#ifndef INCLUDED_GREXTRAS_UHD_CONTROL_PORT_HPP
#define INCLUDED_GREXTRAS_UHD_CONTROL_PORT_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The UHD control port block gives property tree access to a USRP object.
 * This allows other user blocks to programmatically access USRP properties,
 * through the GRAS properties interface and element tree API.
 *
 * Available properties:
 * - "command_time" of type uhd::time_spec_t
 *   (note, set to time_spec_t(0.0) to clear command time)
 * - "rx_freq" of type uhd::tune_request
 * - "tx_freq" of type uhd::tune_request
 * - "rx_gain" of type double
 * - "tx_gain" of type double
 * - "time_source" of type string
 * - "clock_source" of type string
 * - "time_now" of type uhd::time_spec_t
 * - "time_pps" of type uhd::time_spec_t
 *   (getting time pps gets the time at the last pps)
 *   (setting time pps sets the time at the next pps)
 *
 * Documentation for these properties can be found
 * by inspecting the uhd/usrp/multi_usrp.hpp header.
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
