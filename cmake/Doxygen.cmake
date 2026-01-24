#==============================================================================
# Doxygen.cmake
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

#==============================================================================
# Function: enable_doxygen
# Description: This function enables the building of Doxygen documentation
#              if the ENABLE_DOXYGEN option is set to ON.
#==============================================================================

function(enable_doxygen)
    # Option to enable Doxygen documentation builds
    option(ENABLE_DOXYGEN "Enable Doxygen documentation builds of source" OFF)

    if(ENABLE_DOXYGEN)
        # Doxygen configuration settings
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

        # Find Doxygen and optional components
        find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)

        if(Doxygen_FOUND)
            # Set input and output files for Doxygen configuration
            set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxyfile.in)
            set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

            # Configure the Doxygen file
            configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
            message(STATUS "Doxygen build started...")

            # Custom target to build Doxygen documentation
            add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
            )

            message(STATUS "Doxygen documentation target 'docs' created")
            message(STATUS "Build documentation with: cmake --build . --target docs")
        else()
            message(STATUS "Doxygen not found. Doxygen needs to be installed to generate the documentation.")
        endif()
    endif()
endfunction()

#==============================================================================
# Usage:
# 1. Include this script in your main CMakeLists.txt:
#       include(cmake/Doxygen.cmake)
# 2. Call the function to enable Doxygen documentation:
#       enable_doxygen()
# 3. Configure the build with the option ENABLE_DOXYGEN=ON to generate documentation:
#       cmake -DENABLE_DOXYGEN=ON ..
# 4. Build documentation:
#       cmake --build . --target docs
#==============================================================================
