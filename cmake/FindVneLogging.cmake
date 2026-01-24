#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# FindVneLogging.cmake
#
# Finds the VneLogging library and sets up the imported target.
#
# This module defines:
#   VneLogging_FOUND - True if VneLogging is found
#   VneLogging_INCLUDE_DIRS - Include directories for VneLogging
#   VneLogging_LIBRARIES - Libraries to link against
#
# Usage:
#   find_package(VneLogging REQUIRED)
#   target_link_libraries(your_target PRIVATE vne::logging)
#==============================================================================

include(FindPackageHandleStandardArgs)
include(CMakeFindDependencyMacro)

# Try to find the config file first (for installed packages)
find_package(VneLogging QUIET NO_MODULE PATHS
    ${CMAKE_CURRENT_LIST_DIR}/../..
    ${CMAKE_CURRENT_LIST_DIR}/../../..
    ${CMAKE_INSTALL_PREFIX}
    ${CMAKE_INSTALL_PREFIX}/lib/cmake/VneLogging
)

if(VneLogging_FOUND)
    return()
endif()

# If config file not found, try to find the library manually
find_path(VneLogging_INCLUDE_DIR
    NAMES vertexnova/logging/logging.h
    PATHS
        ${CMAKE_CURRENT_LIST_DIR}/../../src
        ${CMAKE_CURRENT_LIST_DIR}/../../include
        ${CMAKE_INSTALL_PREFIX}/include
        /usr/local/include
        /usr/include
)

find_library(VneLogging_LIBRARY
    NAMES VneLogging vnelogging
    PATHS
        ${CMAKE_CURRENT_LIST_DIR}/../../lib
        ${CMAKE_INSTALL_PREFIX}/lib
        /usr/local/lib
        /usr/lib
)

# Handle the QUIETLY and REQUIRED arguments and set VneLogging_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(VneLogging
    FOUND_VAR VneLogging_FOUND
    REQUIRED_VARS
        VneLogging_LIBRARY
        VneLogging_INCLUDE_DIR
)

if(VneLogging_FOUND)
    # Create imported target
    if(NOT TARGET vne::logging)
        add_library(vne::logging STATIC IMPORTED)
        set_target_properties(vne::logging PROPERTIES
            IMPORTED_LOCATION "${VneLogging_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${VneLogging_INCLUDE_DIR}"
        )

        # Link pthread on non-Windows platforms
        if(NOT WIN32)
            set_target_properties(vne::logging PROPERTIES
                INTERFACE_LINK_LIBRARIES "pthread"
            )
        endif()
    endif()

    # Set variables for backward compatibility
    set(VneLogging_LIBRARIES ${VneLogging_LIBRARY})
    set(VneLogging_INCLUDE_DIRS ${VneLogging_INCLUDE_DIR})
endif()

mark_as_advanced(VneLogging_INCLUDE_DIR VneLogging_LIBRARY)
