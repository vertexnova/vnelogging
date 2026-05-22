#==============================================================================
# 06_platform_logging - Cross-platform logging initialisation example
#
# This example runs as a standalone binary on desktop platforms only
# (macOS, Linux, Windows).  On iOS, visionOS, Android, and Web it is
# excluded because those targets are integrated into app bundles, not
# standalone executables.  The bridging snippets in main.cpp show how
# to wire up logging from the platform layer on those targets.
#==============================================================================

if(NOT VNE_PLATFORM_IOS AND NOT VNE_PLATFORM_VISIONOS
   AND NOT VNE_PLATFORM_ANDROID AND NOT VNE_PLATFORM_WEB)

    add_executable(06_PlatformLogging main.cpp)

    target_link_libraries(06_PlatformLogging
        PRIVATE
            vne::logging
    )

    target_include_directories(06_PlatformLogging
        PRIVATE
            $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
            $<BUILD_INTERFACE:${VNE_SRC_DIR}>
    )

endif()
