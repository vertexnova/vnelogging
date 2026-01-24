#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

#==============================================================================
# ProjectSetup.cmake
#==============================================================================

#==============================================================================
# Default Build Type Configuration
#==============================================================================

# Set a default build type if none was specified
set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(DEFAULT_BUILD_TYPE "Debug")
endif()
message("Default build type: ${DEFAULT_BUILD_TYPE}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${DEFAULT_BUILD_TYPE}"
        CACHE STRING "Choose the type of build." FORCE
    )
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
    )
endif()

#==============================================================================
# Output Directory Configuration
#==============================================================================

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Directory for runtime output")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Directory for library output")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Directory for archive output")

#==============================================================================
# Clang-based Tools Configuration
#==============================================================================

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#==============================================================================
# Interprocedural Optimization (IPO) / Link Time Optimization (LTO) Configuration
#==============================================================================

option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" ON)
if(ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
    if(ipo_supported)
        # Enable LTO only for Release builds to preserve debug symbols in Debug builds
        if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
            message(STATUS "Interprocedural Optimization (LTO) enabled for ${CMAKE_BUILD_TYPE} build")
        else()
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
            message(STATUS "Interprocedural Optimization (LTO) disabled for ${CMAKE_BUILD_TYPE} build to preserve debug symbols")
        endif()
    else()
        message(WARNING "IPO is not supported: ${ipo_output}")
    endif()
endif()
