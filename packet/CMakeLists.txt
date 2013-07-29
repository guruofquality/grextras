########################################################################
## GrExtras Packet Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        scrambler.cpp scrambler.xml
        descrambler.cpp descrambler.xml
        burst_tagger.cpp burst_tagger.xml
        packet_framer.py packet_framer.xml
        packet_deframer.py packet_deframer.xml
        framer_sink.cpp #helper block for packet deframer
    TARGET extras_packet
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
    ${extras_packet_LOCATION}
    ${extras_misc_LOCATION}
    ${extras_math_LOCATION}
    ${extras_sources_LOCATION}
)
string(REPLACE ";" "${GRAS_TOOL_PATH_SEP}" GRAS_MODULE_PATH "${GRAS_MODULE_PATH}")
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${GRAS_MODULE_PATH}")

list(APPEND GRAS_PYTHON_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_framer.py
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_deframer.py
)
string(REPLACE ";" "${GRAS_TOOL_PATH_SEP}" GRAS_PYTHON_PATH "${GRAS_PYTHON_PATH}")
list(APPEND GR_TEST_ENVIRONS "GRAS_PYTHON_PATH=${GRAS_PYTHON_PATH}")

#register unit tests
GR_ADD_TEST(packet_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packet_test.py)
GR_ADD_TEST(scrambler_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scrambler_test.py)
