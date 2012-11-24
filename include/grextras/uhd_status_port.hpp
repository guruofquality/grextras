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

#ifndef INCLUDED_GREXTRAS_UHD_STATUS_PORT_HPP
#define INCLUDED_GREXTRAS_UHD_STATUS_PORT_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>
#include <string>

namespace grextras
{

/*!
 * The UHD status port provides asynchronous status data about a USRP device.
 * This block has one output port that produces message.
 * The format of the messages is independent from UHD types.
 * The following information is provided by a message:
 * 
 * The key = "async_metadata".
 * This is a USRP message read from recv_async_msg().
 * The value will be a PMCDict where the key-values are:
 *  - "channel" : size_t
 *  - "time_spec" : PMCTuple(uint64, double)
 *  - "event_code" : int
 *  - "user_payload" : std::vector<uint32_t>
 * Note: time_spec will not be present if have_time_spec == False
 * 
 * The key = <name of a sensor>
 * These values are polled from get_sensor().
 * The value can be a string, bool, signed, or double.
 * This block will not automatically poll the sensors.
 * Sensors have to be explicitly enabled via add_sensor().
 */
struct GREXTRAS_API UHDStatusPort : virtual gras::Block
{
    typedef boost::shared_ptr<UHDStatusPort> sptr;

    /*!
     * Create a new UHD message source block.
     * The device address represents a device on your system.
     * To be portable, the addr is in the comma delimited format.
     * Ex: make("addr=192.168.10.2") 
     * http://files.ettus.com/uhd_docs/manual/html/identification.html
     */
    static sptr make(const std::string &addr);

    /*!
     * Add to the list of sensors to query.
     */
    virtual void add_sensor(const std::string &name) = 0;
};

}


#endif /*INCLUDED_GREXTRAS_UHD_STATUS_PORT_HPP*/
