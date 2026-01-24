#==============================================================================
# Examples CMakeLists.txt
#==============================================================================

# 01 - Basic logging: Simple hello world with logging
add_subdirectory(01_basic)

# 02 - Multiple loggers: Different subsystems with different settings
add_subdirectory(02_multiple_loggers)

# 03 - Combined logging: library with vne::logging + app with spdlog
add_subdirectory(03_combined_logging)

# 04 - Performance test: Benchmarking sync vs async logging
add_subdirectory(04_performance_test)
