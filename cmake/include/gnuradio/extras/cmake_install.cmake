# Install script for directory: /home/john/src/grextras/include/gnuradio/extras

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_devel")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gnuradio/extras" TYPE FILE FILES
    "/home/john/src/grextras/include/gnuradio/extras/uhd_amsg_source.h"
    "/home/john/src/grextras/include/gnuradio/extras/api.h"
    "/home/john/src/grextras/include/gnuradio/extras/add.h"
    "/home/john/src/grextras/include/gnuradio/extras/add_const.h"
    "/home/john/src/grextras/include/gnuradio/extras/delay.h"
    "/home/john/src/grextras/include/gnuradio/extras/divide.h"
    "/home/john/src/grextras/include/gnuradio/extras/multiply.h"
    "/home/john/src/grextras/include/gnuradio/extras/multiply_const.h"
    "/home/john/src/grextras/include/gnuradio/extras/noise_source.h"
    "/home/john/src/grextras/include/gnuradio/extras/signal_source.h"
    "/home/john/src/grextras/include/gnuradio/extras/subtract.h"
    "/home/john/src/grextras/include/gnuradio/extras/stream_selector.h"
    "/home/john/src/grextras/include/gnuradio/extras/decim_fir.h"
    "/home/john/src/grextras/include/gnuradio/extras/blob_to_filedes.h"
    "/home/john/src/grextras/include/gnuradio/extras/blob_to_socket.h"
    "/home/john/src/grextras/include/gnuradio/extras/blob_to_stream.h"
    "/home/john/src/grextras/include/gnuradio/extras/filedes_to_blob.h"
    "/home/john/src/grextras/include/gnuradio/extras/socket_to_blob.h"
    "/home/john/src/grextras/include/gnuradio/extras/stream_to_blob.h"
    "/home/john/src/grextras/include/gnuradio/extras/tuntap.h"
    "/home/john/src/grextras/include/gnuradio/extras/msg_many_to_one.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "extras_devel")

