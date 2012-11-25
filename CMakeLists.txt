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

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-fvisibility=hidden)
    add_definitions(-fvisibility-inlines-hidden)
endif()

########################################################################
# Find GrExtras build dependencies
########################################################################
if(NOT GRAS_FOUND)
    find_package(GRAS)
endif()
if(NOT GRAS_FOUND)
    message(FATAL_ERROR "GR Advanced Scheduler not found - required for GrExtras")
endif()

find_package(Boost)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost C++ headers not found - required for GrExtras")
endif()

if(NOT VOLK_FOUND)
    find_package(Volk)
endif()
if(NOT VOLK_FOUND)
    message(WARNING "VOLK vector library not found - optional for math blocks")
endif()

if(NOT GNURADIO_CORE_FOUND)
    find_package(GnuradioCore)
endif()
if(NOT GNURADIO_CORE_FOUND)
    message(WARNING "GNU Radio Core library not found - optional for noise source")
endif()

find_package(UHD)
if(NOT UHD_FOUND)
    message(WARNING "USRP hardware driver not found - optional for USRP async message")
endif()

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/grextras)
add_subdirectory(lib)
add_subdirectory(python/grextras)
add_subdirectory(grc)
add_subdirectory(scripts)
add_subdirectory(tests)
