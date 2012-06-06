# Copyright 2011-212 Free Software Foundation, Inc.
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
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

########################################################################
# Setup swig generation
########################################################################
set(GR_SWIG_INCLUDE_DIRS
    ${GR_EXTRAS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
)

#assume super tree
if(ENABLE_EXTRAS)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/gruel/src/swig)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/gruel/src/swig)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/swig)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/gr-uhd/swig)
endif()

if(UHD_FOUND)
    list(APPEND GR_SWIG_FLAGS "-DHAVE_UHD")
endif()

foreach(incdir ${GNURADIO_CORE_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/swig)
endforeach(incdir)

foreach(incdir ${GRUEL_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gruel/swig)
endforeach(incdir)

set(GR_SWIG_LIBRARIES gnuradio-extras)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/extras_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/extras)

GR_SWIG_MAKE(extras_swig extras_swig.i)

########################################################################
# Install the built swig module
########################################################################
GR_SWIG_INSTALL(
    TARGETS extras_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/extras
    COMPONENT "extras_python"
)

########################################################################
# Install swig .i files for development
########################################################################
if(UHD_FOUND)
    list(APPEND swig_sources
        extras_uhd_amsg_source.i
    )
endif()

list(APPEND swig_sources
    extras_swig.i
    extras_factory.i
    extras_delay.i
    extras_sources.i
    extras_ops.i
    extras_stream_selector.i
    extras_block_gateway.i
    extras_fir.i
    extras_pmt.i
    extras_blobs.i
    ${CMAKE_CURRENT_BINARY_DIR}/extras_swig_doc.i
)

install(
    FILES ${swig_sources}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "extras_swig"
)
