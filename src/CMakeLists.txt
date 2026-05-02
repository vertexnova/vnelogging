#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Define header files (internal headers in src/)
set(INCLUDE_FILES
    vertexnova/logging/core/log_level.h
    vertexnova/logging/core/time_stamp.h
    vertexnova/logging/core/log_sink.h
    vertexnova/logging/core/console_log_sink.h
    vertexnova/logging/core/file_log_sink.h
    vertexnova/logging/core/log_formatter.h
    vertexnova/logging/core/log_stream.h
    vertexnova/logging/core/text_color.h
    vertexnova/logging/core/log_queue.h
    vertexnova/logging/core/log_queue_worker.h
    vertexnova/logging/core/log_dispatcher.h
    vertexnova/logging/core/logger_controller.h
    vertexnova/logging/core/logger.h
    vertexnova/logging/core/sync_logger.h
    vertexnova/logging/core/async_logger.h
    vertexnova/logging/log_manager.h
)

# Public headers in include/ directory
set(PUBLIC_HEADERS
    ${VNE_INCLUDE_DIR}/vertexnova/logging/export.h
    ${VNE_INCLUDE_DIR}/vertexnova/logging/logging.h
    ${VNE_INCLUDE_DIR}/vertexnova/logging/utils/path_utils.h
)

# Define source files
set(SOURCE_FILES
    vertexnova/logging/core/console_log_sink.cpp
    vertexnova/logging/core/file_log_sink.cpp
    vertexnova/logging/core/log_formatter.cpp
    vertexnova/logging/core/log_stream.cpp
    vertexnova/logging/core/text_color.cpp
    vertexnova/logging/core/log_queue.cpp
    vertexnova/logging/core/log_queue_worker.cpp
    vertexnova/logging/core/log_dispatcher.cpp
    vertexnova/logging/core/logger_controller.cpp
    vertexnova/logging/core/sync_logger.cpp
    vertexnova/logging/core/async_logger.cpp
    vertexnova/logging/log_manager.cpp
    vertexnova/logging/logging.cpp
)

#==============================================================================
#                                 Library                                      #
#==============================================================================

add_library(vnelogging ${_VNE_LOGGING_LIB_KIND} ${SOURCE_FILES} ${INCLUDE_FILES} ${PUBLIC_HEADERS})
vnelogging_apply_shared_export(vnelogging)

# Alias for the library
add_library(vne::logging ALIAS vnelogging)

# Set interface include directories for the library using generator expressions
target_include_directories(vnelogging
    PUBLIC
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific library linking (Threads::Threads is portable for pthread)
if(NOT WIN32 AND NOT EMSCRIPTEN)
    target_link_libraries(vnelogging PRIVATE Threads::Threads)
endif()

# Mobile platform specific linking
if(VNE_TARGET_PLATFORM STREQUAL "Android")
    target_link_libraries(vnelogging PRIVATE log)
elseif(VNE_TARGET_PLATFORM STREQUAL "iOS" OR VNE_TARGET_PLATFORM STREQUAL "visionOS")
    # Link Foundation as a framework for iOS/visionOS builds.
    target_link_options(vnelogging PRIVATE "-framework" "Foundation")
endif()

# MSVC-specific: Suppress getenv deprecation warning (C4996)
if(MSVC)
    target_compile_definitions(vnelogging PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Set compile features
target_compile_features(vnelogging PUBLIC cxx_std_20)

# Link against warnings, build settings, and common utilities
target_link_libraries(vnelogging PUBLIC vne::logging::Warnings vne::logging::BuildSettings vne::common)
