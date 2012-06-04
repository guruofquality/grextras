# Copyright 2012 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup extras as a subproject
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("extras" ENABLE_EXTRAS)

if(ENABLE_EXTRAS)

include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_EXTRAS_DESCRIPTION "Misc GNU Radio Blocks")

CPACK_COMPONENT("extras_runtime"
    GROUP        "Extras"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("extras_devel"
    GROUP        "Extras"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("extras_python"
    GROUP        "Extras"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;extras_runtime"
)

CPACK_COMPONENT("extras_swig"
    GROUP        "Extras"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;extras_python;extras_devel"
)

add_subdirectory(${GR_EXTRAS_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/grextras)
endif(ENABLE_EXTRAS)
