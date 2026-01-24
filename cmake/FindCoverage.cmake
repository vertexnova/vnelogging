#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# FindCoverage.cmake
# Module to handle coverage settings for different platforms
#==============================================================================

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    if(UNIX AND NOT APPLE)
        # Linux: Use gcov/lcov
        find_program(LCOV_EXECUTABLE lcov)
        find_program(GENHTML_EXECUTABLE genhtml)
        find_program(GCOV_EXECUTABLE gcov)

        if(NOT LCOV_EXECUTABLE)
            message(FATAL_ERROR "lcov not found! Please install it: sudo apt-get install lcov")
        endif()
        if(NOT GENHTML_EXECUTABLE)
            message(FATAL_ERROR "genhtml not found! Please install it: sudo apt-get install lcov")
        endif()
        if(NOT GCOV_EXECUTABLE)
            message(FATAL_ERROR "gcov not found! Please install it")
        endif()

        message(STATUS "Configuring code coverage for Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")

    elseif(APPLE)
        # macOS: Use LLVM coverage
        message(STATUS "Configuring code coverage for macOS/iOS")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")

    elseif(WIN32)
        message(WARNING "Coverage on Windows is not fully supported yet")
        # Windows coverage would require additional setup
    endif()

    message(STATUS "Code coverage enabled")
endif()
