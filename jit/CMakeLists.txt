########################################################################
# Setup OpenCL
########################################################################
find_package(OpenCL)
if(OPENCL_FOUND)
    include_directories(${OPENCL_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${OPENCL_LIBRARIES})
    add_definitions(-DHAVE_OPENCL)
    #the following definition is supposed to be a harmless fix
    #http://www.khronos.org/message_boards/showthread.php/7931
    add_definitions(-DCL_USE_DEPRECATED_OPENCL_1_1_APIS)
else()
    message(WARNING "OpenCL library not found - optional for OpenCL block")
endif()

########################################################################
# Setup ORC
########################################################################
find_package(ORC)
if(ORC_FOUND)
    include_directories(${ORC_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${ORC_LIBRARIES})
    add_definitions(-DHAVE_ORC)
else()
    message(WARNING "ORC library not found - optional for ORC block")
endif()

########################################################################
# Setup Clang
########################################################################
find_program(LLVM_CONFIG_EXECUTABLE llvm-config DOC "llvm-config executable")
if(LLVM_CONFIG_EXECUTABLE)
    find_package(LLVM)
endif()
if(LLVM_FOUND)
    find_program(CLANG_EXECUTABLE clang DOC "clang executable")
    set(CLANG_FOUND ${CLANG_EXECUTABLE})
else()
    message(WARNING "LLVM library not found - optional for Clang block")
endif()
if(NOT CLANG_FOUND)
    message(WARNING "Clang executable not found - optional for Clang block")
endif()

if(CLANG_FOUND)
    add_definitions(${LLVM_CFLAGS})
    link_directories(${LLVM_LIBRARY_DIRS})
    include_directories(${LLVM_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${LLVM_MODULE_LIBS})
    list(APPEND GRAS_TOOL_LIBRARIES ${LLVM_LDFLAGS})
    SET_SOURCE_FILES_PROPERTIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/clang_block.cpp
        PROPERTIES COMPILE_DEFINITIONS "CLANG_EXECUTABLE=${CLANG_EXECUTABLE}"
    )
    add_definitions(-DHAVE_CLANG)
endif()

########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_TOOL_LIBRARIES ${Boost_LIBRARIES})

########################################################################
## GrExtras JIT Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        orc_block.cpp orc_block.xml
        #opencl_block.cpp opencl_block.xml
        #clang_block.cpp clang_block.xml
    TARGET extras_jit
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
    ${extras_jit_LOCATION}
)
string(REPLACE ";" "${GRAS_TOOL_PATH_SEP}" GRAS_MODULE_PATH "${GRAS_MODULE_PATH}")
list(APPEND GR_TEST_ENVIRONS "GRAS_MODULE_PATH=${GRAS_MODULE_PATH}")

#register unit tests
if(ORC_FOUND)
GR_ADD_TEST(orc_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/orc_test.py)
endif()

if(OPENCL_FOUND)
GR_ADD_TEST(opencl_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/opencl_test.py)
endif()

if(CLANG_FOUND)
GR_ADD_TEST(clang_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/clang_test.py)
endif()
