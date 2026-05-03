# Clean-tree smoke test: only installed prefix + this directory (no vnelogging source).
cmake_minimum_required(VERSION 3.19)
project(vnelogging_package_consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(VneLogging CONFIG REQUIRED)

if(NOT TARGET vne::vnecommon)
    message(FATAL_ERROR "Expected vne::vnecommon after find_package(VneLogging) (vnecommon transitive export)")
endif()
if(NOT TARGET vne::logging)
    message(FATAL_ERROR "Expected vne::logging after find_package(VneLogging)")
endif()

add_executable(consumed_pkg_test main.cpp)
target_link_libraries(consumed_pkg_test PRIVATE vne::logging)
