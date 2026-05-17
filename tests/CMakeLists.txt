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

project(TestVneLogging)

# Disable logging tests for iOS and Web platforms (IO module disabled, file operations not available)
if(VNE_TARGET_PLATFORM STREQUAL "iOS" OR VNE_TARGET_PLATFORM STREQUAL "Web")
    message(STATUS "Logging tests disabled for ${VNE_TARGET_PLATFORM} (IO module disabled)")
    return()
endif()

#==============================================================================
# Setup Google Test
#==============================================================================

# Default: bundled googletest (static) so TestVneLogging runs from the terminal
# without DYLD_LIBRARY_PATH. System GTest is often shared (@rpath) and fails at
# runtime when launched outside Qt Creator / ctest.
option(VNE_USE_SYSTEM_GTEST "Use system-installed Google Test instead of bundled deps/external/googletest" OFF)

if(VNE_USE_SYSTEM_GTEST)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        message(STATUS "Using system Google Test (VNE_USE_SYSTEM_GTEST=ON)")
        set(GTEST_LIBRARIES GTest::gtest_main GTest::gmock)
    else()
        message(WARNING "VNE_USE_SYSTEM_GTEST is ON but GTest was not found; falling back to bundled Google Test")
    endif()
endif()

if(NOT VNE_USE_SYSTEM_GTEST OR NOT GTest_FOUND)
    if(EXISTS "${CMAKE_SOURCE_DIR}/deps/external/googletest/CMakeLists.txt")
        message(STATUS "Using bundled Google Test from deps/external (static)")
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/deps/external/googletest ${CMAKE_BINARY_DIR}/deps/external/googletest)
        include(GoogleTest)
        set(GTEST_LIBRARIES gtest_main gmock)
        set(GTEST_INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/deps/external/googletest/googletest/include
            ${CMAKE_SOURCE_DIR}/deps/external/googletest/googlemock/include
        )
    else()
        message(STATUS "deps/external/googletest not found, using FetchContent")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0
        )
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        set(GTEST_LIBRARIES gtest_main gmock)
        set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include ${googletest_SOURCE_DIR}/googlemock/include)
    endif()
endif()

#==============================================================================
#                              Source and Headers                              #
#==============================================================================

# Define includes and sources for the test executable
set(TEST_INCLUDES
    core/mocks/log_sink_mock.h
    core/mocks/time_stamp_provider_mock.h
    stream_redirect.h
)

set(TEST_SOURCES
    core/log_level_test.cpp
    core/time_stamp_test.cpp
    core/console_log_sink_test.cpp
    core/file_log_sink_test.cpp
    core/log_formatter_test.cpp
    core/text_color_test.cpp
    core/log_stream_test.cpp
    core/log_queue_test.cpp
    core/log_queue_worker_test.cpp
    core/log_dispatcher_test.cpp
    core/logger_controller_test.cpp
    core/sync_logger_test.cpp
    core/async_logger_test.cpp
    core/logger_performance_test.cpp
    log_manager_test.cpp
    logging_system_test.cpp
    logging_path_test.cpp
    log_test.cpp
    main.cpp
)

#==============================================================================
#                              Build Executable                                #
#==============================================================================

# Create the test executable
add_executable(TestVneLogging ${TEST_INCLUDES} ${TEST_SOURCES})

# Include directories
target_include_directories(TestVneLogging
    SYSTEM PUBLIC
        ${GTEST_INCLUDE_DIRS}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)

# Link libraries
target_link_libraries(TestVneLogging
    PRIVATE
        ${GTEST_LIBRARIES}
        vne::logging
)

# When vnelogging is a shared library, ensure the test binary finds it at runtime in the build tree.
if(VNE_LOGGING_LIB_TYPE STREQUAL "shared")
    if(WIN32)
        add_custom_command(TARGET TestVneLogging POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:vnelogging>
                $<TARGET_FILE_DIR:TestVneLogging>
            VERBATIM)
    else()
        # Match the real output directory of vnelogging (not a guessed ../lib relative to the test exe).
        set_target_properties(TestVneLogging PROPERTIES
            BUILD_RPATH "$<TARGET_FILE_DIR:vnelogging>"
        )
    endif()
endif()

# Web platform specific configuration
if(VNE_TARGET_PLATFORM STREQUAL "Web")
    target_compile_definitions(TestVneLogging PRIVATE VNE_PLATFORM_WEB)
    set_target_properties(TestVneLogging PROPERTIES
        SUFFIX ".js"
    )
endif()

# Add a test with verbose output
add_test(NAME vnelogging.test COMMAND TestVneLogging --gtest_print_time=1)

# Set test properties
set_tests_properties(vnelogging.test PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:TestVneLogging>"
)
