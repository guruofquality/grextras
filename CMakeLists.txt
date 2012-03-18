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
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.6)
project(gr-extras CXX)
enable_testing()

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

########################################################################
# Compiler specific setup
########################################################################
if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
    add_definitions(-fvisibility-inlines-hidden)
endif()

########################################################################
# Find boost
########################################################################
if(UNIX AND EXISTS "/usr/lib64")
    list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
endif(UNIX AND EXISTS "/usr/lib64")
set(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
find_package(Boost "1.35")

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile howto")
endif()

########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX
set(GR_RUNTIME_DIR      bin)
set(GR_LIBRARY_DIR      lib${LIB_SUFFIX})
set(GR_INCLUDE_DIR      include)
set(GR_DATA_DIR         share)
set(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/gnuradio)
set(GR_DOC_DIR          ${GR_DATA_DIR}/doc)
set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/gnuradio)
set(GR_CONF_DIR         etc)
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/gnuradio/conf.d)
set(GR_LIBEXEC_DIR      libexec)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/gnuradio)
set(GRC_BLOCKS_DIR      ${GR_PKG_DATA_DIR}/grc/blocks)

########################################################################
# Find gnuradio build dependencies
########################################################################
find_package(Volk)
find_package(Gruel)
find_package(GnuradioCore)

if(NOT VOLK_FOUND)
    message(FATAL_ERROR "VOLK required to compile extras")
endif()

if(NOT GRUEL_FOUND)
    message(FATAL_ERROR "Gruel required to compile extras")
endif()

if(NOT GNURADIO_CORE_FOUND)
    message(FATAL_ERROR "GnuRadio Core required to compile extras")
endif()

########################################################################
# feature check
########################################################################
include(GrPython)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from gruel import pmt; exit(hasattr(pmt, 'pmt_make_mgr'))"
    RESULT_VARIABLE HAVE_PMT_EXTENSIONS_TEST_OUT
)

if (HAVE_PMT_EXTENSIONS_TEST_OUT EQUAL 1)
    message(STATUS "PMT extensions feature detected...")
    set(HAVE_PMT_EXTENSIONS TRUE)
else()
    message(STATUS "PMT extensions feature missing...")
    set(HAVE_PMT_EXTENSIONS FALSE)
endif()

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from gnuradio import gr; exit(hasattr(gr.top_block, 'msg_connect'))"
    RESULT_VARIABLE HAVE_MSG_PASSING_TEST_OUT
)

if (HAVE_MSG_PASSING_TEST_OUT EQUAL 1)
    message(STATUS "Message passing feature detected...")
    set(HAVE_MSG_PASSING TRUE)
else()
    message(STATUS "Message passing feature missing...")
    set(HAVE_MSG_PASSING FALSE)
endif()

if(HAVE_MSG_PASSING)
    add_definitions(-DHAVE_MSG_PASSING)
endif()

#todo real check
if(WIN32)
    add_definitions(-DHAVE_IO_H)
endif()

########################################################################
# Optional req UHD
########################################################################
find_package(UHD)

if(UHD_FOUND AND HAVE_MSG_PASSING)
    include_directories(${UHD_INCLUDE_DIRS})
    link_directories(${UHD_LIBRARY_DIRS})
endif()

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    ${Boost_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${GRUEL_INCLUDE_DIRS}
    ${GNURADIO_CORE_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${VOLK_LIBRARY_DIRS}
    ${GRUEL_LIBRARY_DIRS}
    ${GNURADIO_CORE_LIBRARY_DIRS}
)

set(GR_EXTRAS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/extras)
add_subdirectory(lib)
add_subdirectory(swig)
add_subdirectory(python)
add_subdirectory(grc)
