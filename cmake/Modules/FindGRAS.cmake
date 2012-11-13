########################################################################
# Find the library for the GNU Radio Advanced Scheduler
########################################################################

include(FindPkgConfig)
PKG_CHECK_MODULES(PC_GRAS gras)

find_path(
    GRAS_INCLUDE_DIRS
    NAMES gras/gras.hpp
    HINTS $ENV{GRAS_DIR}/include
        ${PC_GRAS_INCLUDEDIR}
    PATHS /usr/local/include
          /usr/include
)

find_library(
    GRAS_LIBRARIES
    NAMES gras
    HINTS $ENV{GRAS_DIR}/lib
        ${PC_GRAS_LIBDIR}
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRAS DEFAULT_MSG GRAS_LIBRARIES GRAS_INCLUDE_DIRS)
mark_as_advanced(GRAS_LIBRARIES GRAS_INCLUDE_DIRS)
