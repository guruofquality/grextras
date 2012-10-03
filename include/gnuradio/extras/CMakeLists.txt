# Copyright 2011-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install public header files
########################################################################

if(UHD_FOUND)
    list(APPEND include_sources
        uhd_amsg_source.h
    )
endif()

list(APPEND include_sources
    api.h
    add.h
    add_const.h
    delay.h
    divide.h
    multiply.h
    multiply_const.h
    noise_source.h
    signal_source.h
    subtract.h
    stream_selector.h
    blob_to_filedes.h
    blob_to_socket.h
    blob_to_stream.h
    filedes_to_blob.h
    socket_to_blob.h
    stream_to_blob.h
    tuntap.h
    msg_many_to_one.h
    socket_msg.h
)

install(
    FILES ${include_sources}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/extras
    COMPONENT "extras_devel"
)
