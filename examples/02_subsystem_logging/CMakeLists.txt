#==============================================================================
# 02_subsystem_logging - Game engine style subsystem logging
#==============================================================================

add_executable(02_SubsystemLogging main.cpp)

target_link_libraries(02_SubsystemLogging
    PRIVATE
        vne::logging
)

target_include_directories(02_SubsystemLogging
    PRIVATE
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${VNE_SRC_DIR}>
)
