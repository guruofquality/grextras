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
# Setup python install
########################################################################
include(GrPython)

if(HAVE_PMT_EXTENSIONS AND HAVE_MSG_PASSING)
    list(APPEND python_sources
        pkt2.py
    )
endif()

list(APPEND python_sources
    __init__.py
    block_gateway.py
)

GR_PYTHON_INSTALL(
    FILES ${python_sources}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/extras
    COMPONENT "extras_python"
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

if(HAVE_PMT_EXTENSIONS AND HAVE_MSG_PASSING)
    list(APPEND extras_qa_sources
        qa_msg_passing.py
    )
endif()

list(APPEND extras_qa_sources
    qa_add_and_friends.py
    qa_block_gateway.py
    qa_delay.py
    qa_noise_source.py
    qa_signal_source.py
)

foreach(py_qa_test_file ${extras_qa_sources})
    set(py_qa_test_file ${CMAKE_CURRENT_SOURCE_DIR}/${py_qa_test_file})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    set(GR_TEST_PYTHON_DIRS
        ${CMAKE_BINARY_DIR}/swig
    )
    set(GR_TEST_TARGET_DEPS gnuradio-extras)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} ${py_qa_test_file})
endforeach(py_qa_test_file)
