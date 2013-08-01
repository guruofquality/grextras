########################################################################
## GrExtras Sources Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        signal_source.cpp signal_source.xml
        noise_source.cpp noise_source.xml
    TARGET extras_sources
    DIRECTORY extras
    COMPONENT extras
)

########################################################################
# Unit tests!
########################################################################
include(GRASTest)

#register unit tests
GRAS_ADD_TEST(noise_source_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/noise_source_test.py)
GRAS_ADD_TEST(signal_source_test ${GRAS_TEST_PYEXE} ${CMAKE_CURRENT_SOURCE_DIR}/signal_source_test.py)

