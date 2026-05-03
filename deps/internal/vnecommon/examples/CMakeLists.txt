#==============================================================================
# VneCommon Examples
#==============================================================================

# Macro example: platform/compiler/arch macros, assertions, utility macros
add_executable(vnecommon_example
    macro_example/main.cpp
)
target_link_libraries(vnecommon_example PRIVATE vne::common)

# NonCopyable / NonMovable / NonCopyableNonMovable example
add_executable(vnecommon_noncopyable_example
    noncopyable_example/main.cpp
)
target_link_libraries(vnecommon_noncopyable_example PRIVATE vne::common)

# Set output directory for all example binaries
set_target_properties(
    vnecommon_example
    vnecommon_noncopyable_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
