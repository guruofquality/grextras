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
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${GRAS_TOOL_MODULE_PATH}")

#register unit tests
GR_ADD_TEST(delay_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/delay_test.py)
GR_ADD_TEST(datagram_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/datagram_test.py)
GR_ADD_TEST(stream_selector_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/stream_selector_test.py)
