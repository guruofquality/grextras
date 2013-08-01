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
        block_tree.xml
    TARGET extras_misc
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GRASTest)

#register unit tests
GRAS_ADD_TEST(delay_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/delay_test.py)
GRAS_ADD_TEST(datagram_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/datagram_test.py)
GRAS_ADD_TEST(stream_selector_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/stream_selector_test.py)
