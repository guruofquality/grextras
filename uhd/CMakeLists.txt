########################################################################
# Setup UHD
########################################################################
if(NOT DEFINED UHD_FOUND)
    find_package(UHD)
endif()

if(UHD_FOUND)
    include_directories(${UHD_INCLUDE_DIRS})
    link_directories(${UHD_LIBRARY_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${UHD_LIBRARIES})
    add_definitions(-DHAVE_UHD)
else()
    message(WARNING "USRP hardware driver not found - optional for USRP port blocks")
endif()

########################################################################
## GrExtras UHD Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        uhd_control_port.cpp uhd_control_port.xml
        uhd_status_port.cpp uhd_status_port.xml
    TARGET extras_uhd
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# setup SWIG build
########################################################################
include(GRASSwig)
if(UHD_FOUND)
    list(APPEND GRAS_SWIG_TOOL_INCLUDES ${UHD_INCLUDE_DIRS})
    list(APPEND GRAS_SWIG_TOOL_LIBRARIES ${UHD_LIBRARIES})
    list(APPEND CMAKE_SWIG_FLAGS "-DHAVE_UHD")
endif()
GRAS_SWIG_TOOL(
    SOURCE UHDTypes.i
    TARGET UHDTypes
    DIRECTORY extras
    COMPONENT extras
)
