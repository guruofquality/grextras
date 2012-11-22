// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_TUNTAP_HPP
#define INCLUDED_GREXTRAS_TUNTAP_HPP

#include <grextras/config.hpp>
#include <gras/hier_block.hpp>
#include <string>

namespace grextras
{

/*!
 * Tun/Tap message block: LINUX ONLY!
 *
 * This block provides input and output message ports
 * that connect up directly with an internal tun tap fd.
 *
 * Use the Universal TUN/TAP device driver to move packets to/from kernel
 * See /usr/src/linux/Documentation/networking/tuntap.txt
 *
 * Tuntap should not be used to create a MAC layer!
 * https://en.wikipedia.org/wiki/Transmission_Control_Protocol#TCP_over_wireless_networks
 *
 * The input port is a message port.
 * Each incoming message is a tag where
 *  - key is set to "datagram"
 *  - the value is a gras::SBuffer
 * Other Input messages are dropped.
 *
 * The output port is a message port.
 * Each outgoing message is a tag where
 *  - key is set to "datagram"
 *  - the value is a gras::SBuffer
 */
struct GREXTRAS_API TunTap : virtual gras::HierBlock
{
    typedef boost::shared_ptr<TunTap> sptr;

    /*!
     * Make a new TUN/TAP message interface block.
     * \param dev the device name (depends on os, blank for automatic)
     * \return a new TUN/TAP interface block
     */
    static sptr make(const std::string &dev = "");

    virtual std::string get_dev_name(void) = 0;
};

}

#endif /*INCLUDED_GREXTRAS_TUNTAP_HPP*/
