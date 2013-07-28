########################################################################
## GrExtras Misc Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        datagram_to_stream.cpp datagram_to_stream.xml
        stream_to_datagram.cpp stream_to_datagram.xml
        stream_selector.cpp stream_selector.xml
        time_align.cpp time_align.xml
        delay.cpp delay.xml
        query_server.xml
    TARGET extras_misc
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GrTest)
list(APPEND GR_TEST_TARGET_DEPS extras_misc)
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

#setup path for this module
get_target_property(module_location extras_misc LOCATION)
string(REPLACE "$(Configuration)" ${CMAKE_BUILD_TYPE} module_location ${module_location})
file(TO_NATIVE_PATH "${module_location}" module_location)
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${module_location}")

#register unit tests
GR_ADD_TEST(delay_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/delay_test.py)
GR_ADD_TEST(datagram_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/datagram_test.py)
GR_ADD_TEST(stream_selector_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/stream_selector_test.py)
