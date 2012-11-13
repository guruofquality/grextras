
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    PROGRAMS
    grcc
    DESTINATION bin
    COMPONENT "extras_python"
)
