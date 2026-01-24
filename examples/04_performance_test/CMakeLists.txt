#==============================================================================
# 04_performance_test - Performance benchmarking example
#==============================================================================

add_executable(04_PerformanceTest main.cpp)

target_link_libraries(04_PerformanceTest
    PRIVATE
        vne::logging
)

target_include_directories(04_PerformanceTest
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
