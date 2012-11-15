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
# setup SWIG build
########################################################################
list(APPEND GR_SWIG_INCLUDE_DIRS ${GREXTRAS_SOURCE_DIR}/include)
list(APPEND GR_SWIG_INCLUDE_DIRS ${GRAS_INCLUDE_DIRS})
list(APPEND GR_SWIG_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(GR_SWIG_LIBRARIES grextras ${GRAS_LIBRARIES})

GR_SWIG_MAKE(GrExtras_Misc GrExtras_Misc.i)
GR_SWIG_INSTALL(
    TARGETS
    GrExtras_Misc
    DESTINATION ${GR_PYTHON_DIR}/grextras
    COMPONENT "extras_python"
)

########################################################################
# Install rules
########################################################################
GR_PYTHON_INSTALL(
    FILES __init__.py
    DESTINATION ${GR_PYTHON_DIR}/grextras
    COMPONENT "extras_python"
)
