########################################################################
# Setup OpenCL
########################################################################
find_package(OpenCL)
if(OPENCL_FOUND)
    include_directories(${OPENCL_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${OPENCL_LIBRARIES})
    add_definitions(-DHAVE_OPENCL)
    #the following definition is supposed to be a harmless fix
    #http://www.khronos.org/message_boards/showthread.php/7931
    add_definitions(-DCL_USE_DEPRECATED_OPENCL_1_1_APIS)
else()
    message(WARNING "OpenCL library not found - optional for OpenCL block")
endif()

########################################################################
# Setup ORC
########################################################################
find_package(ORC)
if(ORC_FOUND)
    include_directories(${ORC_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${ORC_LIBRARIES})
    add_definitions(-DHAVE_ORC)
else()
    message(WARNING "ORC library not found - optional for ORC block")
endif()

########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_TOOL_LIBRARIES ${Boost_LIBRARIES})

########################################################################
## GrExtras JIT Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        orc_block.cpp orc_block.xml
        opencl_block.cpp opencl_block.xml
        clang_block.xml
    TARGET extras_jit
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GRASTest)

#register unit tests
if(ORC_FOUND)
GRAS_ADD_TEST(orc_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/orc_test.py)
endif()

if(OPENCL_FOUND)
GRAS_ADD_TEST(opencl_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/opencl_test.py)
endif()
