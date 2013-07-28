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

if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    add_definitions(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    add_definitions(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
endif(MSVC)

########################################################################
# Import the GRAS Tool
########################################################################
if(GRAS_ROOT)
    list(APPEND CMAKE_MODULE_PATH ${GRAS_ROOT}/share/gras/cmake/Modules)
elseif($ENV{GRAS_ROOT})
    list(APPEND CMAKE_MODULE_PATH $ENV{GRAS_ROOT}/share/gras/cmake/Modules)
else(GRAS_ROOT)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/gras/cmake/Modules)
endif(GRAS_ROOT)
include(GRASTool)

#gives us the test utils helper
list(APPEND GR_TEST_PYTHON_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/python)

add_subdirectory(math)
add_subdirectory(misc)

GRAS_TOOL(
    SOURCES block_tree.xml
    TARGET extras_block_tree
    DIRECTORY extras
    COMPONENT extras
)

return()

########################################################################
# Find GrExtras build dependencies
########################################################################
if(NOT DEFINED GRAS_FOUND)
    find_package(GRAS)
endif()
if(NOT GRAS_FOUND)
    message(FATAL_ERROR "GR Advanced Scheduler not found - required for GrExtras")
endif()

if(NOT DEFINED PMC_FOUND)
    find_package(PMC)
endif()
if(NOT PMC_FOUND)
    message(FATAL_ERROR "Polymorphic container library not found - required for GrExtras")
endif()

find_package(Boost)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost C++ headers not found - required for GrExtras")
endif()

if(NOT DEFINED VOLK_FOUND)
    find_package(Volk)
endif()
if(NOT VOLK_FOUND)
    message(WARNING "VOLK vector library not found - optional for math blocks")
endif()

find_package(UHD)
if(NOT UHD_FOUND)
    message(WARNING "USRP hardware driver not found - optional for USRP port blocks")
endif()

find_package(OpenCL)
if (NOT OPENCL_FOUND)
    message(WARNING "OpenCL library not found - optional for OpenCL block")
endif()

find_package(ORC)
if (NOT ORC_FOUND)
    message(WARNING "ORC library not found - optional for ORC block")
endif()

find_program(LLVM_CONFIG_EXECUTABLE llvm-config DOC "llvm-config executable")
if(LLVM_CONFIG_EXECUTABLE)
    find_package(LLVM)
endif()
if(LLVM_FOUND)
    find_program(CLANG_EXECUTABLE clang DOC "clang executable")
    set(CLANG_FOUND ${CLANG_EXECUTABLE})
else()
    message(WARNING "LLVM library not found - optional for Clang block")
endif()
if(NOT CLANG_FOUND)
    message(WARNING "Clang executable not found - optional for Clang block")
endif()

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/grextras)
add_subdirectory(lib)
add_subdirectory(python/grextras)
add_subdirectory(grc)
add_subdirectory(tests)
