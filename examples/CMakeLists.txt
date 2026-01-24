#==============================================================================
# Examples CMakeLists.txt
#==============================================================================

# 01 - Hello Logging: Simple getting started example
add_subdirectory(01_hello_logging)

# 02 - Subsystem Logging: Game engine style with different subsystem loggers
add_subdirectory(02_subsystem_logging)

# 03 - spdlog Integration: Library with vne::logging + app with spdlog
add_subdirectory(03_spdlog_integration)

# 04 - Benchmark: Performance testing sync vs async logging
add_subdirectory(04_benchmark)
