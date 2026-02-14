#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Project declaration
project(vnelogging VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

#==============================================================================
# Add custom CMake modules path
#==============================================================================
# VneCMake shared modules (submodule)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/vnecmake/modules")
# Project-specific modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#==============================================================================
# Internal Libraries (deps/internal) - use if already in build, else add
#==============================================================================
set(VNE_DEPS_INTERNAL_DIR ${PROJECT_SOURCE_DIR}/deps/internal)
include(VneUseDep)

set(_vnelogging_build_examples_saved ${BUILD_EXAMPLES})
vne_use_dep(TARGET vne::common SUBDIR "${VNE_DEPS_INTERNAL_DIR}/vnecommon" BINARY_DIR "${CMAKE_BINARY_DIR}/deps/internal/vnecommon"
    CACHE_VARS BUILD_EXAMPLES OFF)
set(BUILD_EXAMPLES ${_vnelogging_build_examples_saved} CACHE BOOL "" FORCE)

#==============================================================================
# Project Setup
#==============================================================================
include(ProjectSetup)

#==============================================================================
# Platform Detection
#==============================================================================

# Allow manual override of target platform first
if(DEFINED VNE_TARGET_PLATFORM)
    message(STATUS "Using manually specified target platform: ${VNE_TARGET_PLATFORM}")
else()
    # Detect the platform and set platform-specific macros
    if(EMSCRIPTEN)
        set(VNE_TARGET_PLATFORM "Web")
    elseif(WIN32)
        set(VNE_TARGET_PLATFORM "Windows")
    elseif(APPLE)
        if(VISIONOS OR ${CMAKE_SYSTEM_NAME} STREQUAL "visionOS")
            set(VNE_TARGET_PLATFORM "visionOS")
        elseif(IOS OR ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
            set(VNE_TARGET_PLATFORM "iOS")
        else()
            set(VNE_TARGET_PLATFORM "macOS")
        endif()
    elseif(UNIX)
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(VNE_TARGET_PLATFORM "Linux")
        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
            set(VNE_TARGET_PLATFORM "Android")
        else()
            message(FATAL_ERROR "Unsupported UNIX platform: ${CMAKE_SYSTEM_NAME}")
        endif()
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endif()

# Output the detected platform
message(STATUS "Detected platform: ${VNE_TARGET_PLATFORM}")

#==============================================================================
# Platform-specific compile definitions
#==============================================================================

if(VNE_TARGET_PLATFORM STREQUAL "macOS")
    add_compile_definitions(VNE_PLATFORM_MACOS)
    message(STATUS "Defined VNE_PLATFORM_MACOS")
elseif(VNE_TARGET_PLATFORM STREQUAL "iOS")
    add_compile_definitions(VNE_PLATFORM_IOS)
    message(STATUS "Defined VNE_PLATFORM_IOS")
elseif(VNE_TARGET_PLATFORM STREQUAL "visionOS")
    add_compile_definitions(VNE_PLATFORM_VISIONOS)
    message(STATUS "Defined VNE_PLATFORM_VISIONOS")
elseif(VNE_TARGET_PLATFORM STREQUAL "Windows")
    add_compile_definitions(VNE_PLATFORM_WIN)
    message(STATUS "Defined VNE_PLATFORM_WIN")
elseif(VNE_TARGET_PLATFORM STREQUAL "Linux")
    add_compile_definitions(VNE_PLATFORM_LINUX)
    message(STATUS "Defined VNE_PLATFORM_LINUX")
elseif(VNE_TARGET_PLATFORM STREQUAL "Android")
    add_compile_definitions(VNE_PLATFORM_ANDROID)
    message(STATUS "Defined VNE_PLATFORM_ANDROID")
elseif(VNE_TARGET_PLATFORM STREQUAL "Web")
    add_compile_definitions(VNE_PLATFORM_WEB)
    message(STATUS "Defined VNE_PLATFORM_WEB")
endif()

#==============================================================================
# Build Options
#==============================================================================
# Build presets: DEV (local repo) and CI (pipelines). Default DEV when top-level project.
if(CMAKE_PROJECT_NAME STREQUAL "vnelogging")
    set(VNE_LOGGING_DEV_DEFAULT ON)
else()
    set(VNE_LOGGING_DEV_DEFAULT OFF)
endif()
option(VNE_LOGGING_DEV "Dev build: enable examples and tests (local development)" ${VNE_LOGGING_DEV_DEFAULT})
option(VNE_LOGGING_CI "CI build: enable tests, disable examples (e.g. -DVNE_LOGGING_CI=ON in pipelines)" OFF)

# CMake Options:
# | Option            | Default | Description                                                      |
# |-------------------|---------|------------------------------------------------------------------|
# | BUILD_TESTS       | ON      | Build the test suite                                             |
# | VNE_LOGGING_TESTS | ON      | Build vnelogging test suite (set to OFF when used as submodule) |
# | BUILD_EXAMPLES    | OFF     | Build example programs                                           |
# | ENABLE_COVERAGE   | OFF     | Enable code coverage reporting                                   |
option(BUILD_TESTS "Build the test suite" ON)
option(VNE_LOGGING_TESTS "Build vnelogging test suite (turn OFF when used as submodule with only parent tests)" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Apply DEV or CI preset (CI wins over plain defaults; DEV wins over CI if both set)
if(VNE_LOGGING_DEV)
    set(BUILD_TESTS ON CACHE BOOL "Build the test suite" FORCE)
    set(BUILD_EXAMPLES ON CACHE BOOL "Build example programs" FORCE)
    message(STATUS "VneLogging: DEV build -> BUILD_TESTS=ON, BUILD_EXAMPLES=ON")
elseif(VNE_LOGGING_CI)
    set(BUILD_TESTS ON CACHE BOOL "Build the test suite" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    message(STATUS "VneLogging: CI build -> BUILD_TESTS=ON, BUILD_EXAMPLES=OFF")
endif()

# Enable cache system
option(ENABLE_CACHE "Enable compiler cache if available" OFF)
include(CCache)

# Enable static code analysis tools options
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
include(CppCheck)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
include(ClangTidy)

#==============================================================================
# Coverage Support
#==============================================================================

if(ENABLE_COVERAGE)
    include(FindCoverage)
endif()

#==============================================================================
# Documentation Support
#==============================================================================

option(ENABLE_DOXYGEN "Enable Doxygen documentation builds" OFF)
if(ENABLE_DOXYGEN)
    include(Doxygen)
    enable_doxygen()
endif()

#==============================================================================
# Compiler Warnings
#==============================================================================

add_library(Warnings INTERFACE)
include(ProjectWarnings)
set_project_warnings(Warnings)
add_library(vne::logging::Warnings ALIAS Warnings)

#==============================================================================
# Build Settings Interface Library
#==============================================================================

add_library(BuildSettings INTERFACE)
add_library(vne::logging::BuildSettings ALIAS BuildSettings)

# Set compile definitions for the interface library based on the build type
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(BuildSettings INTERFACE VNE_BUILD_TYPE_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(BuildSettings INTERFACE VNE_BUILD_TYPE_RELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    target_compile_definitions(BuildSettings INTERFACE VNE_BUILD_TYPE_RELWITHDEBINFO)
elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    target_compile_definitions(BuildSettings INTERFACE VNE_BUILD_TYPE_MINSIZEREL)
endif()

# Define platform-specific compile definitions
if(${VNE_TARGET_PLATFORM} STREQUAL "Windows")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_WIN)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "Linux")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_LINUX)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "macOS")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_MACOS)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "iOS")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_IOS)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "visionOS")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_VISIONOS)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "Android")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_ANDROID)
elseif(${VNE_TARGET_PLATFORM} STREQUAL "Web")
    target_compile_definitions(BuildSettings INTERFACE VNE_PLATFORM_WEB)
endif()

#==============================================================================
# Include Directories
#==============================================================================

set(VNE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(VNE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(VNE_BUILD_DIR ${CMAKE_BINARY_DIR})
set(VNE_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/deps/external)

#==============================================================================
# Configure config.h
#==============================================================================

configure_file(configs/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories("${CMAKE_BINARY_DIR}")

#==============================================================================
# Add Library
#==============================================================================

add_subdirectory(src)

#==============================================================================
# Tests (enabled only when BUILD_TESTS and VNE_LOGGING_TESTS are ON).
# A parent project may set VNE_LOGGING_TESTS=OFF, but note that presets such as
# VNE_LOGGING_DEV or VNE_LOGGING_CI (when used) configure these options and
# take precedence over individual settings.
#==============================================================================

if(BUILD_TESTS AND VNE_LOGGING_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#==============================================================================
# Examples
#==============================================================================

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#==============================================================================
# Installation
#==============================================================================

include(GNUInstallDirs)

install(TARGETS vnelogging Warnings BuildSettings
    EXPORT VneLoggingTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers from include/
install(DIRECTORY include/vertexnova/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova
    FILES_MATCHING PATTERN "*.h"
)

# Install internal headers from src/ (needed for implementation)
install(DIRECTORY src/vertexnova/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova
    FILES_MATCHING PATTERN "*.h"
)

# Install config.h
install(FILES ${CMAKE_BINARY_DIR}/config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova/logging
)

install(EXPORT VneLoggingTargets
    FILE VneLoggingTargets.cmake
    NAMESPACE vnelogging::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VneLogging
)

# Install CMake module
install(FILES cmake/FindVneLogging.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VneLogging
)
