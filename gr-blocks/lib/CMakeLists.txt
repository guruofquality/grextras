# Copyright 2011 Free Software Foundation, Inc.
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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${VOLK_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_blocks_sources
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
)

list(APPEND gr_blocks_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    volk
)

add_library(gnuradio-blocks SHARED ${gr_blocks_sources})
target_link_libraries(gnuradio-blocks ${gr_blocks_libs})
GR_LIBRARY_FOO(gnuradio-blocks RUNTIME_COMPONENT "blocks_runtime" DEVEL_COMPONENT "blocks_devel")
