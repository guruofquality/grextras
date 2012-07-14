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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

if(UHD_FOUND)
    list(APPEND gr_extras_sources
        uhd_amsg_source.cc
    )
    list(APPEND gr_extras_libs
        ${UHD_LIBRARIES}
    )
endif()

list(APPEND gr_extras_sources
    block.cc
    add.cc
    add_const.cc
    add_const_v.cc
    delay.cc
    divide.cc
    multiply.cc
    multiply_const.cc
    multiply_const_v.cc
    noise_source.cc
    signal_source.cc
    subtract.cc
    stream_selector.cc
    block_gateway.cc
    decim_fir.cc
    pmt_ext_blob.cc
    pmt_ext_mgr.cc
    blob_to_filedes.cc
    blob_to_socket.cc
    blob_to_stream.cc
    filedes_to_blob.cc
    socket_to_blob.cc
    stream_to_blob.cc
    tuntap.cc
    msg_many_to_one.cc
)

list(APPEND gr_extras_libs
    ${Boost_LIBRARIES}
    ${VOLK_LIBRARIES}
    ${GRUEL_LIBRARIES}
    ${GNURADIO_CORE_LIBRARIES}
)

add_library(gnuradio-extras SHARED ${gr_extras_sources})
target_link_libraries(gnuradio-extras ${gr_extras_libs})
set_target_properties(gnuradio-extras PROPERTIES DEFINE_SYMBOL "gnuradio_extras_EXPORTS")

########################################################################
# Install built library files
########################################################################
if (ENABLE_EXTRAS)
    GR_LIBRARY_FOO(gnuradio-extras RUNTIME_COMPONENT "extras_runtime" DEVEL_COMPONENT "extras_devel")
else ()
    install(TARGETS gnuradio-extras
        LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
        ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
        RUNTIME DESTINATION bin              # .dll file
    )
endif ()
