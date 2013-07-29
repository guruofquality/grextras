########################################################################
## GrExtras Packet Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        scrambler.cpp scrambler.xml
        descrambler.cpp descrambler.xml
        burst_tagger.cpp burst_tagger.xml
        #packet_framer.py packet_framer.xml
        #packet_deframer.py packet_deframer.xml
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
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${extras_packet_LOCATION}")

#register unit tests
GR_ADD_TEST(packet_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packet_test.py)
GR_ADD_TEST(scrambler_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scrambler_test.py)
