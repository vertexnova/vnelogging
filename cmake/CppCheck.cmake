#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Option to enable static analysis with cppcheck
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)

if(ENABLE_CPPCHECK)
    # Find the cppcheck executable
    find_program(CMAKE_CXX_CPPCHECK cppcheck)

    if(CMAKE_CXX_CPPCHECK)
        # Check cppcheck version
        execute_process(
            COMMAND ${CMAKE_CXX_CPPCHECK} --version
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            RESULT_VARIABLE CPP_CHECK_RESULT
            OUTPUT_VARIABLE CPP_CHECK_VERSION
            ERROR_VARIABLE CPP_CHECK_ERROR
        )

        if(CPP_CHECK_RESULT EQUAL 0)
            # Append desired arguments to CppCheck
            list(APPEND CMAKE_CXX_CPPCHECK
                --enable=all
                --enable=style
                --suppress=missingIncludeSystem
                --suppress=unusedFunction
                --suppress=unmatchedSuppression
                --inline-suppr
                --inconclusive
                --quiet
                --output-file=${CMAKE_BINARY_DIR}/cppcheck-report.xml
                ${CMAKE_CURRENT_SOURCE_DIR}/src/
            )
            # Note: the option ALL which allows to build the docs together with the application
            add_custom_target(analyze_cppcheck ALL
                COMMAND ${CMAKE_CXX_CPPCHECK}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Cppcheck static code analysis results are saved in ${CMAKE_BINARY_DIR}/cppcheck-report.xml"
            )
            message(STATUS "cppcheck enabled: ${CPP_CHECK_VERSION}")
        else()
            message(SEND_ERROR "Failed to determine cppcheck version: ${CPP_CHECK_ERROR}")
        endif()
    else()
        message(SEND_ERROR "cppcheck requested but executable not found")
    endif()
endif()
