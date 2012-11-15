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
find_package(GRAS)

if(NOT GRAS_FOUND)
    message(FATAL_ERROR "GR Advanced Scheduler not found - required for GrExtras")
endif()

find_package(Boost)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost C++ headers not found - required for GrExtras")
endif()

########################################################################
# Optional req UHD
########################################################################
find_package(UHD)

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
add_subdirectory(include/grextras)
add_subdirectory(lib)
add_subdirectory(python/grextras)
#add_subdirectory(grc)
add_subdirectory(scripts)
