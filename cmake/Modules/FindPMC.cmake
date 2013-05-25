########################################################################
# Find the library for the Polymorphic Container
########################################################################

include(FindPkgConfig)
PKG_CHECK_MODULES(PC_PMC pmc)

find_path(
    PMC_INCLUDE_DIRS
    NAMES PMC/PMC.hpp
    HINTS $ENV{PMC_DIR}/include
        ${PC_PMC_INCLUDEDIR}
    PATHS /usr/local/include
          /usr/include
)

find_library(
    PMC_LIBRARIES
    NAMES pmc
    HINTS $ENV{PMC_DIR}/lib
        ${PC_PMC_LIBDIR}
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PMC DEFAULT_MSG PMC_LIBRARIES PMC_INCLUDE_DIRS)
mark_as_advanced(PMC_LIBRARIES PMC_INCLUDE_DIRS)
