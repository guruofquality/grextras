########################################################################
# Setup library
########################################################################
include_directories(${GREXTRAS_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND GREXTRAS_SOURCES
    delay.cpp
    add.cpp
    multiply.cpp
    subtract.cpp
    divide.cpp
)

########################################################################
# Setup GRAS
########################################################################
include_directories(${GRAS_INCLUDE_DIRS})
link_directories(${GRAS_LIBRARY_DIRS})
list(APPEND GREXTRAS_LIBRARIES ${GRAS_LIBRARIES})

########################################################################
# Setup Boost
########################################################################
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GREXTRAS_LIBRARIES ${Boost_LIBRARIES})

########################################################################
# Setup Volk
########################################################################
include_directories(${VOLK_INCLUDE_DIRS})
link_directories(${VOLK_LIBRARY_DIRS})
list(APPEND GREXTRAS_LIBRARIES ${VOLK_LIBRARIES})

########################################################################
# Optional UHD
########################################################################
if(UHD_FOUND)
    include_directories(${UHD_INCLUDE_DIRS})
    link_directories(${UHD_LIBRARY_DIRS})
    list(APPEND GREXTRAS_LIBRARIES ${UHD_LIBRARIES})
endif()

########################################################################
# Build library
########################################################################
add_library(grextras SHARED ${GREXTRAS_SOURCES})
target_link_libraries(grextras ${GREXTRAS_LIBRARIES})
set_target_properties(grextras PROPERTIES VERSION ${GREXTRAS_VERSION})
set_target_properties(grextras PROPERTIES DEFINE_SYMBOL "GREXTRAS_DLL_EXPORTS")

########################################################################
# Install library
########################################################################
install(TARGETS grextras
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT "extras_runtime" # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "extras_devel"   # .lib file
    RUNTIME DESTINATION bin              COMPONENT "extras_runtime" # .dll file
)

########################################################################
# Build pkg config file
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/grextras.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/grextras.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/grextras.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
    COMPONENT "extras_devel"
)
