########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(grextras CXX C)
enable_testing()

#various compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-Wextra)
endif()

if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    add_definitions(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    add_definitions(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
endif(MSVC)

#add find script modules for build dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

########################################################################
# Import the GRAS Tool
########################################################################
if(GRAS_ROOT)
    #GRAS_ROOT already set
elseif($ENV{GRAS_ROOT})
    set(GRAS_ROOT $ENV{GRAS_ROOT})
else(GRAS_ROOT)
    set(GRAS_ROOT ${CMAKE_INSTALL_PREFIX})
endif(GRAS_ROOT)
list(APPEND CMAKE_MODULE_PATH ${GRAS_ROOT}/share/gras/cmake/Modules)
include(GRASTool)

########################################################################
# Each subdirectory has a collection of blocks
########################################################################
add_subdirectory(math)
add_subdirectory(misc)
add_subdirectory(network)
add_subdirectory(sources)
add_subdirectory(uhd)
add_subdirectory(packet)
add_subdirectory(jit)
