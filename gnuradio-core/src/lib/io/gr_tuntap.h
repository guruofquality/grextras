/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_TUNTAP_H
#define INCLUDED_GR_TUNTAP_H

#include <gr_core_api.h>
#include <gr_hier_block2.h>

class gr_tuntap;

/*!
 * \brief Make a new TUN/TAP block
 *
 * Use the Universal TUN/TAP device driver to move packets to/from kernel
 * See /usr/src/linux/Documentation/networking/tuntap.txt
 *
 * \param dev the device name (depends on os, blank for automatic)
 * \return a new TUN/TAP interface block
 */
GR_CORE_API boost::shared_ptr<gr_tuntap> gr_make_tuntap(const std::string &dev = "");

class GR_CORE_API gr_tuntap : virtual public gr_hier_block2{
    virtual std::string get_dev_name(void) = 0;
};

#endif /* INCLUDED_GR_TUNTAP_H */
