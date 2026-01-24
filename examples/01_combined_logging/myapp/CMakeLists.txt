#==============================================================================
# Example Application using spdlog that links to library using vne::logging
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(01_ExampleCombinedLogging)

# Find or use spdlog from external
if(EXISTS "${CMAKE_SOURCE_DIR}/external/spdlog/CMakeLists.txt")
    message(STATUS "Using spdlog from external directory")
    if(NOT TARGET spdlog::spdlog)
        add_subdirectory(${CMAKE_SOURCE_DIR}/external/spdlog ${CMAKE_BINARY_DIR}/external/spdlog)
    endif()
else()
    # Try to find system spdlog
    find_package(spdlog QUIET)
    if(NOT spdlog_FOUND)
        message(FATAL_ERROR "spdlog not found. Please ensure spdlog is available.")
    endif()
endif()

# Add the application source files
add_executable(01_ExampleCombinedLogging
    main.cpp
)

# Link against the library and spdlog
target_link_libraries(01_ExampleCombinedLogging
    PRIVATE
        mylib
        spdlog::spdlog
)

# Include directories
target_include_directories(01_ExampleCombinedLogging
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard
target_compile_features(01_ExampleCombinedLogging PUBLIC cxx_std_20)
