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
include(GrTest)
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${GRAS_TOOL_MODULE_PATH}")

#register unit tests
GR_ADD_TEST(noise_source_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/noise_source_test.py)
GR_ADD_TEST(signal_source_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/signal_source_test.py)

