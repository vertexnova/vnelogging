#==============================================================================
# 01_hello_logging - Simple getting started example
#==============================================================================

add_executable(01_HelloLogging main.cpp)

target_link_libraries(01_HelloLogging
    PRIVATE
        vne::logging
)

target_include_directories(01_HelloLogging
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
