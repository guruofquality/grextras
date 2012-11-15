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
list(APPEND GR_SWIG_INCLUDE_DIRS ${GRAS_INCLUDE_DIRS})
list(APPEND GR_SWIG_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(GR_SWIG_LIBRARIES gras)

#GR_SWIG_MAKE(Foo Foo.i)
#GR_SWIG_INSTALL(
#    TARGETS
#    Foo
#    DESTINATION ${GR_PYTHON_DIR}/grextras
#    COMPONENT "extras_python"
#)

########################################################################
# Install rules
########################################################################
GR_PYTHON_INSTALL(
    FILES __init__.py
    DESTINATION ${GR_PYTHON_DIR}/grextras
    COMPONENT "extras_python"
)
