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
#   VneLogging_IS_SHARED - TRUE if the found library is a shared library
#
# Usage:
#   find_package(VneLogging REQUIRED)
#   target_link_libraries(your_target PRIVATE vne::logging)
#
# Prefer an installed CMake package (VneLoggingConfig.cmake) from install(EXPORT);
# this module is a fallback for manual prefix layouts.
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

# Guess shared vs static from the resolved library path (manual installs only).
set(VneLogging_IS_SHARED FALSE)
if(VneLogging_LIBRARY)
    if(WIN32)
        get_filename_component(_vlg_lib_dir "${VneLogging_LIBRARY}" DIRECTORY)
        get_filename_component(_vlg_lib_name "${VneLogging_LIBRARY}" NAME_WE)
        if(EXISTS "${_vlg_lib_dir}/${_vlg_lib_name}.dll")
            set(VneLogging_IS_SHARED TRUE)
            set(VneLogging_RUNTIME_LIBRARY "${_vlg_lib_dir}/${_vlg_lib_name}.dll")
        elseif(EXISTS "${_vlg_lib_dir}/../bin/${_vlg_lib_name}.dll")
            set(VneLogging_IS_SHARED TRUE)
            set(VneLogging_RUNTIME_LIBRARY "${_vlg_lib_dir}/../bin/${_vlg_lib_name}.dll")
        endif()
    elseif(APPLE)
        if(VneLogging_LIBRARY MATCHES "\\.dylib$")
            set(VneLogging_IS_SHARED TRUE)
        endif()
    else()
        if(VneLogging_LIBRARY MATCHES "\\.so(\\.[0-9]+)*$")
            set(VneLogging_IS_SHARED TRUE)
        endif()
    endif()
endif()

# Handle the QUIETLY and REQUIRED arguments and set VneLogging_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(VneLogging
    FOUND_VAR VneLogging_FOUND
    REQUIRED_VARS
        VneLogging_LIBRARY
        VneLogging_INCLUDE_DIR
)

if(VneLogging_FOUND)
    if(NOT TARGET vne::logging)
        if(VneLogging_IS_SHARED)
            add_library(vne::logging SHARED IMPORTED)
            if(WIN32 AND DEFINED VneLogging_RUNTIME_LIBRARY)
                set_target_properties(vne::logging PROPERTIES
                    IMPORTED_IMPLIB "${VneLogging_LIBRARY}"
                    IMPORTED_LOCATION "${VneLogging_RUNTIME_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${VneLogging_INCLUDE_DIR}"
                    INTERFACE_COMPILE_DEFINITIONS "VNE_LOGGING_DLL"
                )
            else()
                set_target_properties(vne::logging PROPERTIES
                    IMPORTED_LOCATION "${VneLogging_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${VneLogging_INCLUDE_DIR}"
                    INTERFACE_COMPILE_DEFINITIONS "VNE_LOGGING_DLL"
                )
            endif()
        else()
            add_library(vne::logging STATIC IMPORTED)
            set_target_properties(vne::logging PROPERTIES
                IMPORTED_LOCATION "${VneLogging_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${VneLogging_INCLUDE_DIR}"
            )
        endif()

        find_package(Threads QUIET)
        if(Threads_FOUND AND NOT WIN32 AND NOT EMSCRIPTEN)
            set_property(TARGET vne::logging APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Threads::Threads)
        elseif(NOT WIN32 AND NOT EMSCRIPTEN)
            set_property(TARGET vne::logging APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES "pthread")
        endif()
    endif()

    set(VneLogging_LIBRARIES ${VneLogging_LIBRARY})
    set(VneLogging_INCLUDE_DIRS ${VneLogging_INCLUDE_DIR})
endif()

mark_as_advanced(VneLogging_INCLUDE_DIR VneLogging_LIBRARY)
