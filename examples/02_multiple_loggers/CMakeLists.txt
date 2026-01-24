#==============================================================================
# 02_multiple_loggers - Multiple loggers with different configurations
#==============================================================================

add_executable(02_MultipleLoggers main.cpp)

target_link_libraries(02_MultipleLoggers
    PRIVATE
        vne::logging
)

target_include_directories(02_MultipleLoggers
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
