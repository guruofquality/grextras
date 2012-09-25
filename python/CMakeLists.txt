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

list(APPEND python_sources
    __init__.py
    block_gateway.py
    extras_pmt.py
    pmt_to_python.py
    pmt_rpc.py
    packet_framer.py
    transition_detect.py
    msg_to_stdout.py
    simple_mac.py
    append_key.py
    heart_beat.py
    burst_gate.py
    channel_access_controller.py
    tdma_engine.py
    virtual_channel_formatter.py
    #virtual_channel_mux.py
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

list(APPEND extras_qa_sources
    qa_add_and_friends.py
    qa_block_gateway.py
    qa_delay.py
    qa_noise_source.py
    qa_signal_source.py
    qa_pmt.py
    qa_msg_passing.py
    qa_pmt_to_python.py
    qa_pmt_rpc.py
)

set(GR_TEST_PYTHON_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/../swig
)

#assume super tree
if(ENABLE_EXTRAS)
    list(APPEND GR_TEST_PYTHON_DIRS ${CMAKE_SOURCE_DIR}/gruel/src/python)
    list(APPEND GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/gruel/src/swig)
    list(APPEND GR_TEST_PYTHON_DIRS ${CMAKE_SOURCE_DIR}/gnuradio-core/src/python)
    list(APPEND GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig)
    list(APPEND GR_TEST_TARGET_DEPS volk gruel gnuradio-core)
endif()

foreach(py_qa_test_file ${extras_qa_sources})
    set(py_qa_test_file ${CMAKE_CURRENT_SOURCE_DIR}/${py_qa_test_file})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-extras)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} ${py_qa_test_file})
endforeach(py_qa_test_file)
