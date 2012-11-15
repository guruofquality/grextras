########################################################################
# Unit tests!
########################################################################

include(GrTest)

include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_SOURCE_DIR}/python)
list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_BINARY_DIR}/python/grextras)
list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_BINARY_DIR}/python/grextras/${CMAKE_BUILD_TYPE})
GR_ADD_TEST(import_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/import_test.py)
