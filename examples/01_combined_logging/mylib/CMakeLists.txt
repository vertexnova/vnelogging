#==============================================================================
# Example Library using vne::logging
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(mylib)

# Add the library source files
add_library(mylib STATIC
    my_library.cpp
    my_library.h
)

# Link against vne::logging
target_link_libraries(mylib
    PUBLIC
        vne::logging
)

# Include directories
# The vne::logging target already provides include directories via its PUBLIC interface,
# but we also need to add the source directory explicitly for vertexnova headers
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

# Set C++ standard
target_compile_features(mylib PUBLIC cxx_std_20)
