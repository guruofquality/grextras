########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_TOOL_LIBRARIES ${Boost_LIBRARIES})

########################################################################
## GrExtras Math Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        deserializer.cpp deserializer.xml
        serializer.cpp serializer.xml
        socket_message.cpp socket_message.xml
        tuntap.cpp tuntap.xml
    TARGET extras_network
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GrTest)
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})
list(APPEND GRAS_MODULE_PATH
    ${extras_network_LOCATION}
    ${extras_misc_LOCATION}
)
string(REPLACE ";" "${GRAS_TOOL_PATH_SEP}" GRAS_MODULE_PATH "${GRAS_MODULE_PATH}")
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${GRAS_MODULE_PATH}")

#register unit tests
GR_ADD_TEST(serialize_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/serialize_test.py)
GR_ADD_TEST(socket_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/socket_test.py)
