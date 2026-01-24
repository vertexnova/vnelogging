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
if(VNE_PLATFORM_IOS OR VNE_PLATFORM_WEB)
    message(STATUS "Logging tests disabled for ${CMAKE_SYSTEM_NAME} platform (IO module disabled)")
    return()
endif()

#==============================================================================
# Setup Google Test
#==============================================================================

# Try to find GoogleTest in system first
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Use external googletest if available
    if(EXISTS "${CMAKE_SOURCE_DIR}/external/googletest/CMakeLists.txt")
        message(STATUS "Using Google Test from external directory")
        add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/external/googletest)
        include(GoogleTest)
        # Note: gtest_main already links gtest, so we only need gtest_main and gmock
        set(GTEST_LIBRARIES gtest_main gmock)
        set(GTEST_INCLUDE_DIRS 
            ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include 
            ${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include
        )
    else()
        # Fallback to FetchContent if external not available
        message(STATUS "external/googletest not found, using FetchContent")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
        # Note: gtest_main already links gtest, so we only need gtest_main and gmock
        set(GTEST_LIBRARIES gtest_main gmock)
        set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include ${googletest_SOURCE_DIR}/googlemock/include)
    endif()
else()
    message(STATUS "Using system Google Test")
    set(GTEST_LIBRARIES GTest::gtest_main GTest::gmock)
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
endif()

#==============================================================================
#                              Source and Headers                              #
#==============================================================================

# Define includes and sources for the test executable
set(TEST_INCLUDES
    core/mocks/log_sink_mock.h
    core/mocks/time_stamp_provider_mock.h
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
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)

# Link libraries
target_link_libraries(TestVneLogging
    PRIVATE
        ${GTEST_LIBRARIES}
        vne::logging
)

# Web platform specific configuration
if(VNE_PLATFORM_WEB)
    target_compile_definitions(TestVneLogging PRIVATE VNE_PLATFORM_WEB)
    set_target_properties(TestVneLogging PROPERTIES
        SUFFIX ".js"
    )
endif()

# Add a test
add_test(NAME vnelogging.test COMMAND TestVneLogging)
