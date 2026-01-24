/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Example: Multiple loggers with different configurations
 * Demonstrates game engine-style logging with separate loggers for
 * different subsystems (App, Physics, Render)
 * ----------------------------------------------------------------------
 */

#include <vertexnova/logging/logging.h>

#include <chrono>
#include <filesystem>
#include <thread>

namespace {

// Logger names for different subsystems
constexpr const char* kAppLoggerName = "app";
constexpr const char* kPhysicsLoggerName = "physics";
constexpr const char* kRenderLoggerName = "render";

}  // namespace

// Define custom logging macros for each subsystem
#define APP_LOG_TRACE VNE_LOG_TRACE_L(kAppLoggerName)
#define APP_LOG_DEBUG VNE_LOG_DEBUG_L(kAppLoggerName)
#define APP_LOG_INFO VNE_LOG_INFO_L(kAppLoggerName)
#define APP_LOG_WARN VNE_LOG_WARN_L(kAppLoggerName)
#define APP_LOG_ERROR VNE_LOG_ERROR_L(kAppLoggerName)

#define PHYSICS_LOG_TRACE VNE_LOG_TRACE_L(kPhysicsLoggerName)
#define PHYSICS_LOG_DEBUG VNE_LOG_DEBUG_L(kPhysicsLoggerName)
#define PHYSICS_LOG_INFO VNE_LOG_INFO_L(kPhysicsLoggerName)
#define PHYSICS_LOG_WARN VNE_LOG_WARN_L(kPhysicsLoggerName)
#define PHYSICS_LOG_ERROR VNE_LOG_ERROR_L(kPhysicsLoggerName)

#define RENDER_LOG_TRACE VNE_LOG_TRACE_L(kRenderLoggerName)
#define RENDER_LOG_DEBUG VNE_LOG_DEBUG_L(kRenderLoggerName)
#define RENDER_LOG_INFO VNE_LOG_INFO_L(kRenderLoggerName)
#define RENDER_LOG_WARN VNE_LOG_WARN_L(kRenderLoggerName)
#define RENDER_LOG_ERROR VNE_LOG_ERROR_L(kRenderLoggerName)

namespace {

// Create logger category
CREATE_VNE_LOGGER_CATEGORY("multiple_loggers.example")

}  // namespace

void setupLoggers() {
    // Create logs directory
    const std::string logs_dir = "logs";
    std::filesystem::create_directories(logs_dir);

    // App logger: Console only, INFO level (less verbose)
    vne::log::LoggerConfig app_config;
    app_config.name = kAppLoggerName;
    app_config.sink = vne::log::LogSinkType::eConsole;
    app_config.console_pattern = "[APP] %x [%l] %v";
    app_config.log_level = vne::log::LogLevel::eInfo;
    app_config.async = false;
    vne::log::Logging::configureLogger(app_config);

    // Physics logger: Console + File, DEBUG level (more verbose for debugging physics issues)
    vne::log::LoggerConfig physics_config;
    physics_config.name = kPhysicsLoggerName;
    physics_config.sink = vne::log::LogSinkType::eBoth;
    physics_config.console_pattern = "[PHYSICS] %x [%l] %v";
    physics_config.file_pattern = "[PHYSICS] %x [%l] %v";
    physics_config.file_path = logs_dir + "/physics.log";
    physics_config.log_level = vne::log::LogLevel::eDebug;
    physics_config.async = false;
    vne::log::Logging::configureLogger(physics_config);

    // Render logger: File only, TRACE level (very verbose, written to file only)
    vne::log::LoggerConfig render_config;
    render_config.name = kRenderLoggerName;
    render_config.sink = vne::log::LogSinkType::eFile;
    render_config.file_pattern = "[RENDER] %x [%l] %v";
    render_config.file_path = logs_dir + "/render.log";
    render_config.log_level = vne::log::LogLevel::eTrace;
    render_config.async = true;  // Async for render to avoid blocking the render thread
    vne::log::Logging::configureLogger(render_config);
}

void simulateAppStartup() {
    APP_LOG_INFO << "Application starting...";
    APP_LOG_INFO << "Loading configuration...";
    APP_LOG_INFO << "Initializing subsystems...";
}

void simulatePhysicsUpdate(float delta_time) {
    PHYSICS_LOG_DEBUG << "Physics update started, dt=" << delta_time;
    
    // Simulate some physics calculations
    float gravity = 9.81f;
    float velocity = gravity * delta_time;
    
    PHYSICS_LOG_DEBUG << "Calculated velocity: " << velocity << " m/s";
    
    // Simulate a collision detection
    bool collision_detected = (delta_time > 0.01f);
    if (collision_detected) {
        PHYSICS_LOG_INFO << "Collision detected between objects";
    }
    
    PHYSICS_LOG_DEBUG << "Physics update completed";
}

void simulateRenderFrame(int frame_number) {
    RENDER_LOG_TRACE << "Begin frame " << frame_number;
    
    // Simulate render operations
    RENDER_LOG_TRACE << "Clearing buffers...";
    RENDER_LOG_DEBUG << "Drawing 1000 objects";
    RENDER_LOG_TRACE << "Submitting draw calls...";
    
    // Simulate occasional GPU warning
    if (frame_number % 100 == 0) {
        RENDER_LOG_WARN << "High VRAM usage detected at frame " << frame_number;
    }
    
    RENDER_LOG_TRACE << "End frame " << frame_number;
}

void simulateAppShutdown() {
    APP_LOG_INFO << "Shutting down subsystems...";
    APP_LOG_INFO << "Saving state...";
    APP_LOG_INFO << "Application shutdown complete";
}

int main() {
    // Setup all loggers with different configurations
    setupLoggers();

    // Simulate application lifecycle
    simulateAppStartup();

    APP_LOG_INFO << "Starting main loop...";

    // Simulate a few game frames
    for (int frame = 0; frame < 5; ++frame) {
        // Physics runs at fixed timestep
        simulatePhysicsUpdate(0.016f);  // 60 FPS physics

        // Render runs every frame
        simulateRenderFrame(frame);

        // Small delay to make output readable
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Simulate some issues
    PHYSICS_LOG_WARN << "Physics solver did not converge after 100 iterations";
    RENDER_LOG_ERROR << "Failed to load texture: missing_texture.png";
    APP_LOG_ERROR << "Critical subsystem failure detected";

    simulateAppShutdown();

    // Shutdown all loggers
    vne::log::Logging::shutdown();

    std::cout << "\n=== Example Complete ===" << std::endl;
    std::cout << "Check the following files:" << std::endl;
    std::cout << "  - logs/physics.log (Physics subsystem logs)" << std::endl;
    std::cout << "  - logs/render.log  (Render subsystem logs - most verbose)" << std::endl;
    std::cout << "\nNote: App logs only appear on console (INFO level and above)" << std::endl;

    return 0;
}
