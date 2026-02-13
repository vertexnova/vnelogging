#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(vnecommon VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

#==============================================================================
# Build Options
#==============================================================================

option(BUILD_EXAMPLES "Build example programs" OFF)

#==============================================================================
# Header-Only Library
#==============================================================================

add_library(vnecommon INTERFACE)
add_library(vne::common ALIAS vnecommon)

target_include_directories(vnecommon INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#==============================================================================
# Installation
#==============================================================================

include(GNUInstallDirs)

install(TARGETS vnecommon
    EXPORT VneCommonTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT VneCommonTargets
    FILE VneCommonTargets.cmake
    NAMESPACE vne::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vnecommon
)

#==============================================================================
# Examples
#==============================================================================

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
