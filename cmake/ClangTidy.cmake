#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Enable or disable static analysis with clang-tidy
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)

    if(CLANG_TIDY)
        # Check Clang-tidy version
        execute_process(
            COMMAND ${CLANG_TIDY} --version
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE CLANG_TIDY_RESULT
            OUTPUT_VARIABLE CLANG_TIDY_VERSION
            ERROR_VARIABLE CLANG_TIDY_ERROR
        )

        if(CLANG_TIDY_RESULT EQUAL 0)
            # Set the clang-tidy command with the project binary directory and output format
            set(CLANG_TIDY_OUTPUT_FILE ${CMAKE_BINARY_DIR}/clang-tidy-report.xml)
            set(CLANG_TIDY_CMD
                ${CLANG_TIDY}
                -p=${CMAKE_BINARY_DIR}
                -export-fixes=${CLANG_TIDY_OUTPUT_FILE}
                -format-style=file
                -checks=*
            )
            set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_CMD})
            message(STATUS "clang-tidy enabled: ${CLANG_TIDY_VERSION}")

        else()
            message(WARNING "clang-tidy version check failed: ${CLANG_TIDY_ERROR}")
        endif()
    else()
        message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
endif()
