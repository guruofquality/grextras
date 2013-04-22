########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

########################################################################
# local headers
########################################################################
list(APPEND GR_SWIG_INCLUDE_DIRS ${GREXTRAS_SOURCE_DIR}/include)
list(APPEND GR_SWIG_INCLUDE_DIRS ${GRAS_INCLUDE_DIRS})

########################################################################
# Optional UHD
########################################################################
if(UHD_FOUND AND GNURADIO_CORE_FOUND)
    list(APPEND GR_SWIG_INCLUDE_DIRS ${UHD_INCLUDE_DIRS})
    list(APPEND GR_SWIG_LIBRARIES ${UHD_LIBRARIES})
    list(APPEND GR_SWIG_FLAGS "-DHAVE_UHD")
endif()

########################################################################
# setup SWIG build
########################################################################
list(APPEND GR_SWIG_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(GR_SWIG_LIBRARIES grextras ${GRAS_LIBRARIES})

list(APPEND grextras_swig_targets
    GrExtras_Misc
    GrExtras_Ops
    GrExtras_Sources
    GrExtras_UHDPorts
    GrExtras_UHDTypes
    GrExtras_FramerSink
    GrExtras_OpenCL
)

foreach(target ${grextras_swig_targets})
    GR_SWIG_MAKE(${target} ${target}.i)
endforeach(target)

GR_SWIG_INSTALL(
    TARGETS ${grextras_swig_targets}
    DESTINATION ${GR_PYTHON_DIR}/grextras
    COMPONENT "extras_python"
)

########################################################################
# Install rules
########################################################################
GR_PYTHON_INSTALL(
    FILES

    __init__.py
    GrExtras_TestUtils.py
    GrExtras_Packet.py

    DESTINATION ${GR_PYTHON_DIR}/grextras
    COMPONENT "extras_python"
)
