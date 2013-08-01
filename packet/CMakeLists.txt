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
include(GRASTest)

list(APPEND GRAS_PYTHON_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_framer.py
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_deframer.py
)
string(REPLACE ";" "${ENV_VAR_PATH_SEP}" GRAS_PYTHON_PATH "${GRAS_PYTHON_PATH}")
list(APPEND GRAS_TEST_ENVIRONS "GRAS_PYTHON_PATH=${GRAS_PYTHON_PATH}")

#register unit tests
GRAS_ADD_TEST(packet_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/packet_test.py)
GRAS_ADD_TEST(scrambler_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/scrambler_test.py)
