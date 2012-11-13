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
project(grextras CXX C)
enable_testing()

set(GREXTRAS_VERSION "0.0.0")
set(GREXTRAS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GREXTRAS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${GREXTRAS_SOURCE_DIR}/cmake/Modules)
include(CMakeDefsHelper)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGREXTRAS_DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-fvisibility=hidden)
    add_definitions(-fvisibility-inlines-hidden)
endif()

########################################################################
# Find GrExtras build dependencies
########################################################################
find_package(Volk)
find_package(GRAS)

########################################################################
# Optional req UHD
########################################################################
find_package(UHD)

if(UHD_FOUND)
    include_directories(${UHD_INCLUDE_DIRS})
    link_directories(${UHD_LIBRARY_DIRS})
endif()

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Add subdirectories
########################################################################
#add_subdirectory(include/grextras)
#add_subdirectory(lib)
#add_subdirectory(python)
#add_subdirectory(grc)
add_subdirectory(scripts)
