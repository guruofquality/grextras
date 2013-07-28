########################################################################
# Setup Volk
########################################################################
if(NOT DEFINED VOLK_FOUND)
    find_package(Volk)
endif()

if(VOLK_FOUND)
    include_directories(${VOLK_INCLUDE_DIRS})
    link_directories(${VOLK_LIBRARY_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${VOLK_LIBRARIES})
    add_definitions(-DHAVE_VOLK)
else()
    message(WARNING "VOLK vector library not found - optional for math blocks")
endif()

########################################################################
## GrExtras Math Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        subtract.cpp subtract.xml
        #divide.cpp divide.xml
        #add.cpp add.xml
        #multiply.cpp multiply.xml
        #add_const.cpp add_const_v.cpp add_const.xml
        #multiply_const.cpp multiply_const_v.cpp multiply_const.xml
    TARGET extras_math
    DIRECTORY extras
    COMPONENT extras
)

