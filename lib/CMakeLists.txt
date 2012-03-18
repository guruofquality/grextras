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

if(HAVE_PMT_EXTENSIONS AND HAVE_MSG_PASSING)
    list(APPEND gr_extras_sources
        blob_to_filedes.cc
        blob_to_socket.cc
        blob_to_stream.cc
        filedes_to_blob.cc
        socket_to_blob.cc
        stream_to_blob.cc
        tuntap.cc
    )
endif()

list(APPEND gr_extras_sources
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
    stream_selector.cc
    subtract.cc
    block_gateway.cc
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
install(TARGETS gnuradio-extras
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)
