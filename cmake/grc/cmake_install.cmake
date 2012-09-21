# Install script for directory: /home/john/src/grextras/grc

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/john/src/grextras/grc/extras_uhd_amsg_source.xml"
    "/home/john/src/grextras/grc/extras_add_const.xml"
    "/home/john/src/grextras/grc/extras_block_tree.xml"
    "/home/john/src/grextras/grc/extras_divide.xml"
    "/home/john/src/grextras/grc/extras_multiply.xml"
    "/home/john/src/grextras/grc/extras_signal_source.xml"
    "/home/john/src/grextras/grc/extras_subtract.xml"
    "/home/john/src/grextras/grc/extras_add.xml"
    "/home/john/src/grextras/grc/extras_delay.xml"
    "/home/john/src/grextras/grc/extras_multiply_const.xml"
    "/home/john/src/grextras/grc/extras_noise_source.xml"
    "/home/john/src/grextras/grc/extras_decim_fir.xml"
    "/home/john/src/grextras/grc/extras_blob_to_socket.xml"
    "/home/john/src/grextras/grc/extras_blob_to_stream.xml"
    "/home/john/src/grextras/grc/extras_socket_to_blob.xml"
    "/home/john/src/grextras/grc/extras_stream_to_blob.xml"
    "/home/john/src/grextras/grc/extras_tuntap.xml"
    "/home/john/src/grextras/grc/extras_packet_framer.xml"
    "/home/john/src/grextras/grc/extras_packet_deframer.xml"
    "/home/john/src/grextras/grc/extras_stream_selector.xml"
    "/home/john/src/grextras/grc/extras_pmt_rpc.xml"
    "/home/john/src/grextras/grc/extras_msg_many_to_one.xml"
    "/home/john/src/grextras/grc/extras_transition_detect.xml"
    "/home/john/src/grextras/grc/extras_msg_to_stdout.xml"
    "/home/john/src/grextras/grc/simple_mac.xml"
    "/home/john/src/grextras/grc/adder.xml"
    "/home/john/src/grextras/grc/extras_append_key.xml"
    "/home/john/src/grextras/grc/heart_beat.xml"
    "/home/john/src/grextras/grc/burst_gate.xml"
    "/home/john/src/grextras/grc/channel_access_controller.xml"
    "/home/john/src/grextras/grc/tdma_engine.xml"
    "/home/john/src/grextras/grc/virtual_channel_formatter.xml"
    "/home/john/src/grextras/grc/virtual_channel_mux.xml"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_python")

