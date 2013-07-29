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
        divide.cpp divide.xml
        add.cpp add.xml
        multiply.cpp multiply.xml
        add_const.cpp add_const.xml
        multiply_const.cpp multiply_const.xml
    TARGET extras_math
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GrTest)
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${extras_math_LOCATION}")

#register unit tests
GR_ADD_TEST(math_ops_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/math_ops_test.py)

