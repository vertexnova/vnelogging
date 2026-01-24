#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Define header files
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
    vertexnova/logging/logging.h
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

# Create a static library from the source and header files
add_library(vnelogging STATIC ${SOURCE_FILES} ${INCLUDE_FILES})

# Alias for the library
add_library(vne::logging ALIAS vnelogging)

# Set interface include directories for the library using generator expressions
target_include_directories(vnelogging
    PUBLIC
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific library linking
if(NOT WIN32)
    target_link_libraries(vnelogging PRIVATE pthread)
endif()

# Mobile platform specific linking
if(VNE_TARGET_PLATFORM STREQUAL "Android")
    target_link_libraries(vnelogging PRIVATE log)
elseif(VNE_TARGET_PLATFORM STREQUAL "iOS")
    # iOS-specific libraries if needed
    target_link_libraries(vnelogging PRIVATE Foundation)
endif()

# Set compile features
target_compile_features(vnelogging PUBLIC cxx_std_20)

# Link against warnings and build settings
target_link_libraries(vnelogging PUBLIC vne::logging::Warnings vne::logging::BuildSettings)
