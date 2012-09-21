# Install script for directory: /home/john/src/pre-cog/python

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_python")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/gnuradio/extras" TYPE FILE FILES
    "/home/john/src/pre-cog/python/__init__.py"
    "/home/john/src/pre-cog/python/block_gateway.py"
    "/home/john/src/pre-cog/python/extras_pmt.py"
    "/home/john/src/pre-cog/python/pmt_to_python.py"
    "/home/john/src/pre-cog/python/pmt_rpc.py"
    "/home/john/src/pre-cog/python/packet_framer.py"
    "/home/john/src/pre-cog/python/adder.py"
    "/home/john/src/pre-cog/python/transition_detect.py"
    "/home/john/src/pre-cog/python/msg_to_stdout.py"
    "/home/john/src/pre-cog/python/simple_mac.py"
    "/home/john/src/pre-cog/python/append_key.py"
    "/home/john/src/pre-cog/python/heart_beat.py"
    "/home/john/src/pre-cog/python/burst_gate.py"
    "/home/john/src/pre-cog/python/channel_access_controller.py"
    "/home/john/src/pre-cog/python/tdma_engine.py"
    "/home/john/src/pre-cog/python/virtual_channel_formatter.py"
    "/home/john/src/pre-cog/python/virtual_channel_mux.py"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_python")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_python")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/gnuradio/extras" TYPE FILE FILES
    "/home/john/src/pre-cog/build/python/__init__.pyc"
    "/home/john/src/pre-cog/build/python/block_gateway.pyc"
    "/home/john/src/pre-cog/build/python/extras_pmt.pyc"
    "/home/john/src/pre-cog/build/python/pmt_to_python.pyc"
    "/home/john/src/pre-cog/build/python/pmt_rpc.pyc"
    "/home/john/src/pre-cog/build/python/packet_framer.pyc"
    "/home/john/src/pre-cog/build/python/adder.pyc"
    "/home/john/src/pre-cog/build/python/transition_detect.pyc"
    "/home/john/src/pre-cog/build/python/msg_to_stdout.pyc"
    "/home/john/src/pre-cog/build/python/simple_mac.pyc"
    "/home/john/src/pre-cog/build/python/append_key.pyc"
    "/home/john/src/pre-cog/build/python/heart_beat.pyc"
    "/home/john/src/pre-cog/build/python/burst_gate.pyc"
    "/home/john/src/pre-cog/build/python/channel_access_controller.pyc"
    "/home/john/src/pre-cog/build/python/tdma_engine.pyc"
    "/home/john/src/pre-cog/build/python/virtual_channel_formatter.pyc"
    "/home/john/src/pre-cog/build/python/virtual_channel_mux.pyc"
    "/home/john/src/pre-cog/build/python/__init__.pyo"
    "/home/john/src/pre-cog/build/python/block_gateway.pyo"
    "/home/john/src/pre-cog/build/python/extras_pmt.pyo"
    "/home/john/src/pre-cog/build/python/pmt_to_python.pyo"
    "/home/john/src/pre-cog/build/python/pmt_rpc.pyo"
    "/home/john/src/pre-cog/build/python/packet_framer.pyo"
    "/home/john/src/pre-cog/build/python/adder.pyo"
    "/home/john/src/pre-cog/build/python/transition_detect.pyo"
    "/home/john/src/pre-cog/build/python/msg_to_stdout.pyo"
    "/home/john/src/pre-cog/build/python/simple_mac.pyo"
    "/home/john/src/pre-cog/build/python/append_key.pyo"
    "/home/john/src/pre-cog/build/python/heart_beat.pyo"
    "/home/john/src/pre-cog/build/python/burst_gate.pyo"
    "/home/john/src/pre-cog/build/python/channel_access_controller.pyo"
    "/home/john/src/pre-cog/build/python/tdma_engine.pyo"
    "/home/john/src/pre-cog/build/python/virtual_channel_formatter.pyo"
    "/home/john/src/pre-cog/build/python/virtual_channel_mux.pyo"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_python")

