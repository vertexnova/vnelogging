#==============================================================================
# VneCommon Examples
#==============================================================================

add_executable(vnecommon_example
    main.cpp
)

target_link_libraries(vnecommon_example PRIVATE vne::common)

# Set output directory
set_target_properties(vnecommon_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
