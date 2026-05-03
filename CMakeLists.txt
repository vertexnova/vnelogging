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
cmake_minimum_required(VERSION 3.19)

# Project declaration
project(vnelogging VERSION 1.0.0 LANGUAGES CXX)

# iOS: pick a default install prefix that matches common packaging checks.
# - GitHub Actions sets CI + GITHUB_WORKSPACE; workflows often validate
#   "$GITHUB_WORKSPACE/install/include/..." after `cmake --install` without --prefix.
# - Local/dev builds default to "<binary-dir>/install".
# A relative -DCMAKE_INSTALL_PREFIX=... is resolved relative to CMAKE_BINARY_DIR.
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    if(DEFINED ENV{CI} AND DEFINED ENV{GITHUB_WORKSPACE})
        file(TO_CMAKE_PATH "$ENV{GITHUB_WORKSPACE}" _VNE_GITHUB_WORKSPACE)
        set(_VNE_IOS_CI_INSTALL_PREFIX "${_VNE_GITHUB_WORKSPACE}/install")
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
                OR CMAKE_INSTALL_PREFIX STREQUAL "${CMAKE_BINARY_DIR}/install")
            set(CMAKE_INSTALL_PREFIX "${_VNE_IOS_CI_INSTALL_PREFIX}"
                CACHE PATH "Install prefix for iOS on CI (workspace/install)" FORCE)
        endif()
    elseif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install"
            CACHE PATH "Install prefix for iOS builds (override with -DCMAKE_INSTALL_PREFIX)" FORCE)
    endif()
endif()

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

# Must be defined before add_subdirectory(vnecommon): that subproject registers
# install() when VNE_COMMON_SKIP_INSTALL is OFF; align with this skip flag.
option(VNE_LOGGING_SKIP_INSTALL "Skip install rules when used as a subdependency" OFF)

#==============================================================================
# Internal Libraries (deps/internal) - use if already in build, else add
#==============================================================================
set(VNE_DEPS_INTERNAL_DIR ${PROJECT_SOURCE_DIR}/deps/internal)
# Default: submodule at deps/internal/vnecommon. Override for a sibling/out-of-tree
# clone, e.g. -DVNE_VNECOMMON_ROOT=/Users/you/workspace/github/vne/vnecommon
set(VNE_VNECOMMON_ROOT "${VNE_DEPS_INTERNAL_DIR}/vnecommon" CACHE PATH
    "Path to vnecommon sources (submodule default; set to a standalone repo for local dev)")
include(VneUseDep)

function(_vnelogging_configure_vnecommon_dep)
    set(_vnelogging_build_examples_saved "${BUILD_EXAMPLES}")
    if(VNE_LOGGING_SKIP_INSTALL)
        vne_use_dep(TARGET vne::common SUBDIR "${VNE_VNECOMMON_ROOT}" BINARY_DIR "${CMAKE_BINARY_DIR}/deps/internal/vnecommon"
            CACHE_VARS BUILD_EXAMPLES OFF VNE_COMMON_SKIP_INSTALL ON)
    else()
        vne_use_dep(TARGET vne::common SUBDIR "${VNE_VNECOMMON_ROOT}" BINARY_DIR "${CMAKE_BINARY_DIR}/deps/internal/vnecommon"
            CACHE_VARS BUILD_EXAMPLES OFF)
    endif()
    set(BUILD_EXAMPLES "${_vnelogging_build_examples_saved}" CACHE BOOL "" FORCE)
endfunction()

_vnelogging_configure_vnecommon_dep()
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
# Library type — static or shared (one per build directory)
#==============================================================================
# Parent projects (e.g. VneIo) should align cache: set(VNE_LOGGING_LIB_TYPE "${PARENT_LIB_TYPE}" CACHE STRING "" FORCE)
# before add_subdirectory(vnelogging) when linking vne::logging as a shared dependency.

set(VNE_LOGGING_LIB_TYPE "static" CACHE STRING "VneLogging library type: static or shared (one per build)")
set_property(CACHE VNE_LOGGING_LIB_TYPE PROPERTY STRINGS static shared)
if(NOT VNE_LOGGING_LIB_TYPE STREQUAL "static" AND NOT VNE_LOGGING_LIB_TYPE STREQUAL "shared")
    message(FATAL_ERROR "VNE_LOGGING_LIB_TYPE must be 'static' or 'shared'. Got: ${VNE_LOGGING_LIB_TYPE}")
endif()

if(VNE_LOGGING_LIB_TYPE STREQUAL "shared")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(_VNE_LOGGING_LIB_KIND SHARED)
else()
    set(_VNE_LOGGING_LIB_KIND STATIC)
endif()

message(STATUS "VneLogging: VNE_LOGGING_LIB_TYPE=${VNE_LOGGING_LIB_TYPE}")

function(vnelogging_apply_shared_export target_name)
    get_target_property(_vlg_kind "${target_name}" TYPE)
    if(NOT _vlg_kind STREQUAL "SHARED_LIBRARY")
        return()
    endif()
    target_compile_definitions(${target_name}
        PRIVATE VNE_LOGGING_BUILDING_DLL
        INTERFACE VNE_LOGGING_DLL
    )
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /wd4251 /wd4275)
    else()
        set_target_properties(${target_name} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
        )
    endif()
endfunction()

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

# Ensure CI preset takes precedence over DEV when both are specified.
if(VNE_LOGGING_CI)
    set(VNE_LOGGING_DEV OFF CACHE BOOL "Dev build: enable examples and tests (local development)" FORCE)
endif()
# CMake Options and Presets:
# | Option / Preset            | Default        | Description                                                      |
# |----------------------------|----------------|------------------------------------------------------------------|
# | VNE_LOGGING_DEV            | ON (top-level) | Dev preset: BUILD_TESTS=ON, BUILD_EXAMPLES=ON                    |
# | VNE_LOGGING_CI             | OFF            | CI preset: BUILD_TESTS=ON, BUILD_EXAMPLES=OFF                    |
# | BUILD_TESTS                | ON             | Build the test suite                                             |
# | VNE_LOGGING_TESTS          | ON             | Build vnelogging test suite (can be set to OFF by parent projects) |
# | BUILD_EXAMPLES             | OFF            | Build example programs                                           |
# | ENABLE_COVERAGE            | OFF            | Enable code coverage reporting                                   |
# | VNE_LOGGING_LIB_TYPE       | static         | Library artifact: static or shared                               |
# | VNE_LOGGING_SKIP_INSTALL   | OFF            | Skip install() for vnelogging and bundled vnecommon (subproject) |
# (Declared near the top of this file, before add_subdirectory vnecommon.)
option(BUILD_TESTS "Build the test suite" ON)
option(VNE_LOGGING_TESTS "Build vnelogging test suite (can be set to OFF by parent projects)" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Apply CI or DEV preset (CI takes precedence; DEV is ignored when CI is active)
if(VNE_LOGGING_CI)
    set(BUILD_TESTS ON CACHE BOOL "Build the test suite" FORCE)
    set(VNE_LOGGING_TESTS ON CACHE BOOL "Build vnelogging test suite (turn OFF when used as submodule with only parent tests)")
    set(BUILD_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    # Ensure DEV preset is disabled when CI preset is active
    set(VNE_LOGGING_DEV OFF CACHE BOOL "Disable DEV preset when CI preset is active" FORCE)
    message(STATUS "VneLogging: CI build -> BUILD_TESTS=ON, VNE_LOGGING_TESTS=ON, BUILD_EXAMPLES=OFF")
elseif(VNE_LOGGING_DEV)
    set(BUILD_TESTS ON CACHE BOOL "Build the test suite" FORCE)
    set(VNE_LOGGING_TESTS ON CACHE BOOL "Build vnelogging test suite (turn OFF when used as submodule with only parent tests)")
    set(BUILD_EXAMPLES ON CACHE BOOL "Build example programs" FORCE)
    message(STATUS "VneLogging: DEV build -> BUILD_TESTS=ON, VNE_LOGGING_TESTS=ON, BUILD_EXAMPLES=ON")
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

find_package(Threads REQUIRED)
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
# Rare toolchains leave this unset; packaging expects headers under <prefix>/include/...
if(NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()

if(NOT VNE_LOGGING_SKIP_INSTALL)
    include(CMakePackageConfigHelpers)

    # Installed targets use namespace vne:: (matches README / find_package(VneLogging) consumers).
    # EXPORT_NAME maps the internal vnelogging target to imported target vne::logging; artifact stays OUTPUT_NAME vnelogging.
    set_target_properties(vnelogging PROPERTIES EXPORT_NAME logging)
    set_target_properties(Warnings PROPERTIES EXPORT_NAME logging_Warnings)
    set_target_properties(BuildSettings PROPERTIES EXPORT_NAME logging_BuildSettings)

    # Install the static/shared library artifact (ARCHIVE covers iOS/macOS .a static libs).
    install(TARGETS vnelogging
        EXPORT VneLoggingTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # INTERFACE targets: export only (no lib/ artifact). Kept separate so install rules stay
    # unambiguous for multi-config generators (Xcode) and static archive installs.
    install(TARGETS Warnings BuildSettings
        EXPORT VneLoggingTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install public headers from include/
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/vertexnova/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova
        FILES_MATCHING PATTERN "*.h"
    )

    # Internal API headers from src/ (needed when linking the static library; must populate
    # include/vertexnova/logging for iOS static and other archive installs.)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/vertexnova/logging/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova/logging
        FILES_MATCHING PATTERN "*.h"
    )

    # vnecommon: install headers + VneCommonTargets.cmake via deps/internal/vnecommon
    # (VNE_COMMON_SKIP_INSTALL=OFF). Do not duplicate vertexnova/common headers here.

    # Install config.h
    install(FILES ${CMAKE_BINARY_DIR}/config.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vertexnova/logging
    )

    set(_VNE_LOGGING_CMAKE_PKG_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/VneLogging")
    install(EXPORT VneLoggingTargets
        FILE VneLoggingTargets.cmake
        NAMESPACE vne::
        DESTINATION ${_VNE_LOGGING_CMAKE_PKG_DEST}
    )

    configure_package_config_file(
        cmake/VneLoggingConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/VneLoggingConfig.cmake"
        INSTALL_DESTINATION ${_VNE_LOGGING_CMAKE_PKG_DEST}
    )
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/VneLoggingConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/VneLoggingConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/VneLoggingConfigVersion.cmake"
        DESTINATION ${_VNE_LOGGING_CMAKE_PKG_DEST}
    )

    # Install CMake module (fallback when package config is not used)
    install(FILES cmake/FindVneLogging.cmake
        DESTINATION ${_VNE_LOGGING_CMAKE_PKG_DEST}
    )

    if(EXISTS "${PROJECT_SOURCE_DIR}/LICENSE")
        install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION .)
    endif()
    if(EXISTS "${PROJECT_SOURCE_DIR}/CHANGELOG.md")
        install(FILES "${PROJECT_SOURCE_DIR}/CHANGELOG.md" DESTINATION .)
    endif()
endif()
