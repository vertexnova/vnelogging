#==============================================================================
# 04_benchmark - Performance benchmarking example
#==============================================================================

add_executable(04_Benchmark main.cpp)

target_link_libraries(04_Benchmark
    PRIVATE
        vne::logging
)

target_include_directories(04_Benchmark
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
