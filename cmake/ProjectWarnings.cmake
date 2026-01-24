#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Function to set project-specific compiler warnings
# Arguments:
#   target_name - The name of the target (executable or library)

function(set_project_warnings target_name)
    # Option to treat compiler warnings as errors
    option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

    # MSVC compiler warnings
    set(MSVC_WARNINGS
        /W4           # Baseline reasonable warnings
        /w14242       # 'identifier': conversion from 'type1' to 'type1', possible loss of data
        /w14254       # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
        /w14263       # 'function': member function does not override any base class virtual member function
        /w14265       # 'classname': class has virtual functions, but destructor is not virtual
        /w14287       # 'operator': unsigned/negative constant mismatch
        /we4289       # Nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
        /w14296       # 'operator': expression is always 'boolean_value'
        /w14311       # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545       # Expression before comma evaluates to a function which is missing an argument list
        /w14546       # Function call before comma missing argument list
        /w14547       # 'operator': operator before comma has no effect; expected operator with side-effect
        /w14549       # 'operator': operator before comma has no effect; did you intend 'operator'?
        /w14555       # Expression has no effect; expected expression with side-effect
        /w14619       # Pragma warning: there is no warning number 'number'
        /w14640       # Enable warning on thread un-safe static member initialization
        /w14826       # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior
        /w14905       # Wide string literal cast to 'LPSTR'
        /w14906       # String literal cast to 'LPWSTR'
        /w14928       # Illegal copy-initialization; more than one user-defined conversion has been implicitly applied
        /permissive-  # Standards conformance mode for MSVC compiler
    )

    # Clang and GCC compiler warnings
    set(CLANG_WARNINGS
        -Wall
        -Wextra           # Reasonable and standard
        -Wshadow          # Warn the user if a variable declaration shadows one from a parent context
        -Wnon-virtual-dtor # Warn the user if a class with virtual functions has a non-virtual destructor
        -Wold-style-cast  # Warn for C-style casts
        -Wcast-align      # Warn for potential performance problem casts
        -Wunused          # Warn on anything being unused
        -Woverloaded-virtual # Warn if you overload (not override) a virtual function
        -Wpedantic        # Warn if non-standard C++ is used
        -Wconversion      # Warn on type conversions that may lose data
        -Wsign-conversion # Warn on sign conversions
        -Wnull-dereference # Warn if a null dereference is detected
        -Wdouble-promotion # Warn if float is implicit promoted to double
        -Wformat=2        # Warn on security issues around functions that format output (e.g., printf)
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wmisleading-indentation # Warn if indentation implies blocks where blocks do not exist
        -Wduplicated-cond        # Warn if if/else chain has duplicated conditions
        -Wduplicated-branches    # Warn if if/else branches have duplicated code
        -Wlogical-op             # Warn about logical operations being used where bitwise were probably wanted
        -Wuseless-cast           # Warn if you perform a cast to the same type
    )

    if(WARNINGS_AS_ERRORS)
        list(APPEND CLANG_WARNINGS -Werror)
        list(APPEND MSVC_WARNINGS /WX)
    endif()

    if(MSVC)
        set(PROJECT_WARNINGS ${MSVC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
    else()
        message(WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()

    # Apply the warnings to the target
    target_compile_options(${target_name} INTERFACE ${PROJECT_WARNINGS})
endfunction()
