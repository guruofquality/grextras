########################################################################
# Unit tests!
########################################################################

include(GrTest)

########################################################################
# unit test suite
########################################################################
find_package(Boost COMPONENTS unit_test_framework)

if (NOT Boost_FOUND)
    return()
endif()

set(test_sources
    #empty!
)

include_directories(${GRAS_INCLUDE_DIRS})
include_directories(${PMC_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GR_TEST_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})

#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

#for each source: build an executable, register it as a test
foreach(test_source ${test_sources})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} ${Boost_LIBRARIES} ${GRAS_LIBRARIES})
    GR_ADD_TEST(${test_name}_cpp ${test_name})
endforeach(test_source)

########################################################################
# Python unit tests
########################################################################
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

list(APPEND GR_TEST_TARGET_DEPS grextras)
list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_BINARY_DIR}/python)
list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_BINARY_DIR}/python/grextras)
list(APPEND GR_TEST_PYTHON_DIRS ${GREXTRAS_BINARY_DIR}/python/grextras/${CMAKE_BUILD_TYPE})
GR_ADD_TEST(delay_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/delay_test.py)
GR_ADD_TEST(stream_selector_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/stream_selector_test.py)
GR_ADD_TEST(math_ops_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/math_ops_test.py)
GR_ADD_TEST(signal_source_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/signal_source_test.py)
GR_ADD_TEST(noise_source_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/noise_source_test.py)
GR_ADD_TEST(datagram_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/datagram_test.py)
GR_ADD_TEST(socket_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/socket_test.py)
GR_ADD_TEST(packet_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packet_test.py)
GR_ADD_TEST(scrambler_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scrambler_test.py)
GR_ADD_TEST(serialize_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/serialize_test.py)

if(OPENCL_FOUND)
GR_ADD_TEST(opencl_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/opencl_test.py)
endif(OPENCL_FOUND)

if(ORC_FOUND)
GR_ADD_TEST(orc_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/orc_test.py)
endif(ORC_FOUND)

if(CLANG_FOUND)

    set(CLANG_BLOCK_INCLUDE_DIRS ${GRAS_INCLUDE_DIRS} ${PMC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    string (REPLACE ";" ":" CLANG_BLOCK_INCLUDE_DIRS "${CLANG_BLOCK_INCLUDE_DIRS}")

    list(APPEND GR_TEST_ENVIRONS "CLANG_BLOCK_INCLUDE_DIRS=${CLANG_BLOCK_INCLUDE_DIRS}")
    GR_ADD_TEST(clang_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/clang_test.py)
endif(CLANG_FOUND)
