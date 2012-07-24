/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

#include <gnuradio/extras/tuntap.h>
#include <gnuradio/extras/filedes_to_blob.h>
#include <gnuradio/extras/blob_to_filedes.h>
#include <gr_io_signature.h>
#include <iostream>
#include <boost/format.hpp>

using namespace gnuradio::extras;

static const std::string GROUP_NAME = "blob";

#if defined(linux) || defined(__linux) || defined(__linux__)

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>

int tun_alloc(char *dev, int flags = IFF_TAP | IFF_NO_PI) {

  struct ifreq ifr;
  int fd, err;
  const char *clonedev = "/dev/net/tun";

  /* Arguments taken by the function:
   *
   * char *dev: the name of an interface (or '\0'). MUST have enough
   *   space to hold the interface name if '\0' is passed
   * int flags: interface flags (eg, IFF_TUN etc.)
   */

   /* open the clone device */
   if( (fd = open(clonedev, O_RDWR)) < 0 ) {
     return fd;
   }

   /* preparation of the struct ifr, of type "struct ifreq" */
   memset(&ifr, 0, sizeof(ifr));

   ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

   if (*dev) {
     /* if a device name was specified, put it in the structure; otherwise,
      * the kernel will try to allocate the "next" device of the
      * specified type */
     strncpy(ifr.ifr_name, dev, IFNAMSIZ);
   }

   /* try to create the device */
   if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
     close(fd);
     return err;
   }

  /* if the operation was successful, write back the name of the
   * interface to the variable "dev", so the caller can know
   * it. Note that the caller MUST reserve space in *dev (see calling
   * code below) */
  strcpy(dev, ifr.ifr_name);

  /* this is the special file descriptor that the caller will use to talk
   * with the virtual interface */
  return fd;
}

/***********************************************************************
 * Hier block that combines it all
 **********************************************************************/
class tuntap_impl : public tuntap{
public:
    tuntap_impl(const int fd, const std::string &dev_name):
        gr_hier_block2(
            "tuntap",
            gr_make_io_signature(1, 1, 1),
            gr_make_io_signature(1, 1, 1)
        ),
        _fd(fd),
        _dev_name(dev_name)
    {
        //helpful user message
        std::cout << boost::format(
        "Allocated virtual ethernet interface: %s\n"
        "You must now use ifconfig to set its IP address. E.g.,\n"
        "  $ sudo ifconfig %s 192.168.200.1\n"
        "Be sure to use a different address in the same subnet for each machine.\n"
        ) % get_dev_name() % get_dev_name() << std::endl;

        //make the blocks
        _source = filedes_to_blob::make(_fd);
        _sink = blob_to_filedes::make(_fd);

        //connect
        this->connect(this->self(), 0, _sink, 0);
        this->connect(_source, 0, this->self(), 0);
    }

    ~tuntap_impl(void){
        close(_fd);
    }

    std::string get_dev_name(void){
        return _dev_name;
    }

private:
    filedes_to_blob::sptr _source;
    blob_to_filedes::sptr _sink;
    const int _fd;
    const std::string _dev_name;
};

/***********************************************************************
 * Factory function
 **********************************************************************/
tuntap::sptr tuntap::make(const std::string &dev){
    //make the tuntap
    char dev_cstr[1024];
    strncpy(dev_cstr, dev.c_str(), std::min(sizeof(dev_cstr), dev.size()));
    const int fd = tun_alloc(dev_cstr);
    if (fd <= 0){
        throw std::runtime_error("gr_make_tuntap: tun_alloc failed");
    }
    return gnuradio::get_initial_sptr(new tuntap_impl(fd, dev_cstr));
}

#else //is not a linux

tuntap::sptr tuntap::make(const std::string &){
    throw std::runtime_error("gr_make_tuntap: sorry, not implemented on this OS");
}

#endif //is a linux
