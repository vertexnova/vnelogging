/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Example application using spdlog that links to a library using vne::logging
 * This demonstrates how both logging systems can write to the same file/console
 * ----------------------------------------------------------------------
 */

#include "my_library.h"

#include <iostream>
#include <memory>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace {

// Constants in anonymous namespace with k prefix
constexpr const char* kLogFilePath = "combined.log";
constexpr const char* kConsolePattern = "[SPDLOG-APP] %Y-%m-%d %H:%M:%S.%e [%^%l%$] %v";
constexpr const char* kFilePattern = "[SPDLOG-APP] %Y-%m-%d %H:%M:%S.%e [%n] [%l] %v";
constexpr const char* kLoggerName = "app";

}  // namespace

int main() {
    // Initialize spdlog for the application
    // Create a multi-sink logger that writes to both console and file
    std::vector<spdlog::sink_ptr> sinks;

    // Console sink with color
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern(kConsolePattern);
    sinks.push_back(console_sink);

    // File sink - using the SAME file as vne::logging
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(kLogFilePath, true);
    file_sink->set_level(spdlog::level::debug);
    file_sink->set_pattern(kFilePattern);
    sinks.push_back(file_sink);

    // Create combined logger
    auto combined_logger = std::make_shared<spdlog::logger>(kLoggerName, sinks.begin(), sinks.end());
    combined_logger->set_level(spdlog::level::debug);
    combined_logger->flush_on(spdlog::level::debug);

    // Register as default logger
    spdlog::set_default_logger(combined_logger);

    spdlog::info("Application starting up");
    spdlog::debug("This is a debug message from the application");
    spdlog::default_logger()->flush();

    // Create and initialize library instance (which uses vne::logging)
    example::MyLibrary lib;
    if (!lib.initialize()) {
        spdlog::error("Failed to initialize library");
        return 1;
    }

    spdlog::info("Library initialized, starting operations");
    spdlog::default_logger()->flush();

    // Application logs using spdlog
    spdlog::info("Performing operations with the library");

    // Library operations (will log using vne::logging)
    lib.doWork("operation1");
    spdlog::default_logger()->flush();

    spdlog::warn("This is a warning from the application");
    spdlog::default_logger()->flush();

    lib.processData("sample data");
    spdlog::default_logger()->flush();

    spdlog::error("This is an error message from the application");
    spdlog::default_logger()->flush();

    lib.doWork("operation2");
    spdlog::default_logger()->flush();

    spdlog::info("All operations completed");
    spdlog::default_logger()->flush();

    // Shutdown library
    lib.shutdown();

    spdlog::info("Application shutting down");

    // Flush all logs
    spdlog::shutdown();

    std::cout << "\n=== Check 'combined.log' file to see logs from both systems ===\n";
    std::cout << "You should see:\n";
    std::cout << "  - [SPDLOG-APP] messages from the application (spdlog)\n";
    std::cout << "  - [VNE-LIB] messages from the library (vne::logging)\n";

    return 0;
}
