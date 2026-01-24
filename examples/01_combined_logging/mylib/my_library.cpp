/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 * ----------------------------------------------------------------------
 */

#include "my_library.h"

#include "vertexnova/logging/logging.h"

#include <chrono>
#include <thread>

namespace {

// Constants in anonymous namespace with k prefix
constexpr const char* kLoggerCategory = "mylib";
constexpr const char* kConsolePattern = "[VNE-LIB] %x [%l] [%!] %v";
constexpr const char* kFilePattern = "[VNE-LIB] %x [%n] [%l] [%!] %v";
constexpr const char* kLogFilePath = "combined.log";
constexpr int kWorkDelayMs = 100;
constexpr int kProcessDelayMs = 50;

// Create logger category at namespace scope (following vertexnova pattern)
CREATE_VNE_LOGGER_CATEGORY(kLoggerCategory);

}  // namespace

namespace example {

MyLibrary::~MyLibrary() {
    if (is_initialized_) {
        shutdown();
    }
}

bool MyLibrary::initialize() {
    if (is_initialized_) {
        return true;
    }

    // Initialize vne::logging for the library
    // Configure to write to both console and file
    // Use the default logger name so the macros work correctly
    vne::log::LoggerConfig config;
    config.name = vne::log::kDefaultLoggerName;
    config.sink = vne::log::LogSinkType::eBoth;
    config.console_pattern = kConsolePattern;
    config.file_pattern = kFilePattern;
    config.file_path = kLogFilePath;
    config.log_level = vne::log::LogLevel::eDebug;
    config.flush_level = vne::log::LogLevel::eDebug;
    config.async = false;

    vne::log::Logging::configureLogger(config);

    is_initialized_ = true;

    VNE_LOG_INFO << "MyLibrary initialized successfully";

    return true;
}

void MyLibrary::shutdown() {
    if (!is_initialized_) {
        return;
    }

    VNE_LOG_INFO << "MyLibrary shutting down";
    vne::log::Logging::shutdown();
    is_initialized_ = false;
}

void MyLibrary::doWork(const std::string& operation_name) {
    VNE_LOG_DEBUG << "Starting operation: " << operation_name;

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(kWorkDelayMs));

    VNE_LOG_INFO << "Operation '" << operation_name << "' completed successfully";
}

void MyLibrary::processData(const std::string& data) {
    VNE_LOG_DEBUG << "Processing data: " << data.size() << " bytes";

    if (data.empty()) {
        VNE_LOG_WARN << "Received empty data";
        return;
    }

    // Simulate processing
    std::this_thread::sleep_for(std::chrono::milliseconds(kProcessDelayMs));

    VNE_LOG_INFO << "Data processed successfully: " << data;
}

}  // namespace example
