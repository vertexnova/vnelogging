#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Option to enable or disable compiler cache
option(ENABLE_CACHE "Enable cache if available" OFF)

# If caching is not enabled, exit the script
if(NOT ENABLE_CACHE)
  return()
endif()

# Define the cache option and its possible values
set(CACHE_OPTION "ccache" CACHE STRING "Compiler cache to be used")
set(CACHE_OPTION_VALUES "ccache" "sccache")
set_property(CACHE CACHE_OPTION PROPERTY STRINGS ${CACHE_OPTION_VALUES})

# Check if the specified cache option is supported
list(FIND CACHE_OPTION_VALUES ${CACHE_OPTION} CACHE_OPTION_INDEX)
if(${CACHE_OPTION_INDEX} EQUAL -1)
    message(STATUS "Using custom compiler cache system: '${CACHE_OPTION}', explicitly supported entries are ${CACHE_OPTION_VALUES}")
endif()

# Find the specified cache binary
find_program(CACHE_BINARY ${CACHE_OPTION})
if(CACHE_BINARY)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CACHE_BINARY})
    message("---------------------------------------------------------")
    message(STATUS "${CACHE_OPTION} found and enabled")
    message("---------------------------------------------------------")
else()
    message(WARNING "${CACHE_OPTION} is enabled but was not found. Not using it")
endif()
