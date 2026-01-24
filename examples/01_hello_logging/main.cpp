/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Example: Basic logging usage
 * Demonstrates simple logging setup with console and file output
 * ----------------------------------------------------------------------
 */

#include <vertexnova/logging/logging.h>

#include <filesystem>

namespace {

// Create a logger category for this file
CREATE_VNE_LOGGER_CATEGORY("basic.example")

}  // namespace

int main() {
    // Create logs directory
    const std::string logs_dir = "logs";
    std::filesystem::create_directories(logs_dir);

    // Configure the default logger with console and file output
    vne::log::LoggerConfig config;
    config.name = vne::log::kDefaultLoggerName;
    config.sink = vne::log::LogSinkType::eBoth;
    config.console_pattern = "%x [%l] [%n] %v";
    config.file_pattern = "%x [%l] [%n] [%c] %v";
    config.file_path = logs_dir + "/basic_example.log";
    config.log_level = vne::log::LogLevel::eTrace;
    config.async = false;

    vne::log::Logging::configureLogger(config);

    // Log messages at different levels
    VNE_LOG_TRACE << "This is a trace message - most detailed level";
    VNE_LOG_DEBUG << "This is a debug message - useful for debugging";
    VNE_LOG_INFO << "This is an info message - general information";
    VNE_LOG_WARN << "This is a warning message - something might be wrong";
    VNE_LOG_ERROR << "This is an error message - something went wrong";
    VNE_LOG_FATAL << "This is a fatal message - critical failure";

    // Demonstrate logging with variables
    int count = 42;
    std::string user = "Alice";
    VNE_LOG_INFO << "User: " << user << ", Count: " << count;

    // Demonstrate conditional logging
    bool debug_mode = true;
    if (debug_mode) {
        VNE_LOG_DEBUG << "Debug mode is enabled";
    }

    // Shutdown logging
    vne::log::Logging::shutdown();

    return 0;
}
