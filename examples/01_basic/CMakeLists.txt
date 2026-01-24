#==============================================================================
# 01_basic - Basic logging example
#==============================================================================

add_executable(01_BasicLogging main.cpp)

target_link_libraries(01_BasicLogging
    PRIVATE
        vne::logging
)

target_include_directories(01_BasicLogging
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
