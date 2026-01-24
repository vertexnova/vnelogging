/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   JULY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "logging.h"

#include "core/log_level.h"
#include "core/time_stamp.h"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#ifndef VNE_PLATFORM_WEB
#include <filesystem>
#endif

namespace vne::log {

namespace {

//==============================================================================
// Platform-specific path helpers
//==============================================================================

/**
 * @brief Gets the home directory for the current user.
 * @return Home directory path, or empty string if not found.
 */
std::string getHomeDirectory() {
#if defined(VNE_PLATFORM_WINDOWS)
    if (const char* userprofile = std::getenv("USERPROFILE")) {
        return std::string(userprofile);
    }
#else
    if (const char* home = std::getenv("HOME")) {
        return std::string(home);
    }
#endif
    return "";
}

/**
 * @brief Gets the platform-specific path separator.
 * @return Path separator ("/" or "\\").
 */
constexpr const char* getPathSeparator() {
#if defined(VNE_PLATFORM_WINDOWS)
    return "\\";
#else
    return "/";
#endif
}

/**
 * @brief Checks if a path looks like a build directory.
 * @param path The path to check.
 * @return True if the path appears to be a build directory.
 */
bool isBuildDirectory(const std::string& path) {
    return path.find("build") != std::string::npos 
        || path.find("out") != std::string::npos
        || path.find("bin") != std::string::npos
        || path.find("cmake-build") != std::string::npos;
}

/**
 * @brief Gets the current working directory.
 * @return Current working directory path.
 */
std::string getCurrentDirectory() {
#ifdef VNE_PLATFORM_WEB
    return ".";
#else
    try {
        return std::filesystem::current_path().string();
    } catch (...) {
        return ".";
    }
#endif
}

/**
 * @brief Creates directories recursively (portable).
 * @param path The directory path to create.
 * @return True if successful or directory already exists.
 */
bool createDirectories(const std::string& path) {
#ifdef VNE_PLATFORM_WEB
    return true;  // No filesystem on web
#else
    try {
        if (path.empty()) {
            return false;
        }
        if (std::filesystem::exists(path)) {
            return true;
        }
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
#endif
}

}  // namespace

//==============================================================================
// Static member initialization
//==============================================================================

std::shared_ptr<LogManager> Logging::s_log_manager = nullptr;

//==============================================================================
// Core logging functions
//==============================================================================

void Logging::initialize(const std::string& name, bool async) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->createLogger(name, async);
}

void Logging::shutdown() {
    if (s_log_manager) {
        s_log_manager->finalize();
        s_log_manager.reset();
    }
}

bool Logging::isLoggerAsync(const std::string& logger_name) {
    if (!s_log_manager) {
        return false;
    }
    return s_log_manager->isLoggerAsync(logger_name);
}

void Logging::addConsoleSink(const std::string& logger_name) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->addConsoleSink(logger_name);
}

void Logging::addFileSink(const std::string& logger_name, const std::string& file) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->addFileSink(logger_name, file);
}

void Logging::setConsolePattern(const std::string& logger_name, const std::string& pattern) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->setConsolePattern(logger_name, pattern);
}

void Logging::setFilePattern(const std::string& logger_name, const std::string& pattern) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->setFilePattern(logger_name, pattern);
}

void Logging::setLogLevel(const std::string& logger_name, LogLevel level) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->setLogLevel(logger_name, level);
}

void Logging::setFlushLevel(const std::string& logger_name, LogLevel level) {
    if (!s_log_manager) {
        s_log_manager = std::make_shared<LogManager>();
    }
    s_log_manager->setFlushLevel(logger_name, level);
}

//==============================================================================
// Configuration functions
//==============================================================================

LoggerConfig Logging::defaultLoggerConfig() {
    LoggerConfig config;
    config.name = kDefaultLoggerName;
    config.console_pattern = "%x [%l] %v";
    config.file_pattern = "%x [%n] [%l] [%!] %v";
    config.log_level = LogLevel::eInfo;
    config.flush_level = LogLevel::eError;
    config.async = false;

#ifdef VNE_PLATFORM_WEB
    config.sink = LogSinkType::eConsole;
    config.file_path = "";
#else
    config.sink = LogSinkType::eConsole;
    std::string log_dir = getPlatformSpecificLogDirectory();
    if (!log_dir.empty()) {
        config.file_path = log_dir + getPathSeparator() + "vne.log";
        ensureLogDirectoryExists(log_dir);
    }
#endif

    return config;
}

void Logging::configureLogger(const LoggerConfig& cfg) {
    initialize(cfg.name, cfg.async);

    // Configure console sink
    if (cfg.sink == LogSinkType::eConsole || cfg.sink == LogSinkType::eBoth) {
        addConsoleSink(cfg.name);
        if (!cfg.console_pattern.empty()) {
            setConsolePattern(cfg.name, cfg.console_pattern);
        }
    }

    // Configure file sink (not on web)
#ifndef VNE_PLATFORM_WEB
    if (cfg.sink == LogSinkType::eFile || cfg.sink == LogSinkType::eBoth) {
        if (!cfg.file_path.empty()) {
            addFileSink(cfg.name, cfg.file_path);
            if (!cfg.file_pattern.empty()) {
                setFilePattern(cfg.name, cfg.file_pattern);
            }
        }
    }
#endif

    setLogLevel(cfg.name, cfg.log_level);
    setFlushLevel(cfg.name, cfg.flush_level);
}

//==============================================================================
// Path utility functions
//==============================================================================

std::string Logging::getLogDirectory() {
    // Delegate to platform-specific implementation
    return getPlatformSpecificLogDirectory();
}

std::string Logging::getPlatformSpecificLogDirectory() {
    const std::string sep = getPathSeparator();
    const std::string home = getHomeDirectory();

#if defined(VNE_PLATFORM_WINDOWS)
    // Windows: Use LocalAppData
    if (const char* appdata = std::getenv("LOCALAPPDATA")) {
        return std::string(appdata) + sep + "VertexNova" + sep + "logs";
    }
    if (!home.empty()) {
        return home + sep + "AppData" + sep + "Local" + sep + "VertexNova" + sep + "logs";
    }

#elif defined(VNE_PLATFORM_MACOS)
    // macOS: Use ~/Library/Logs
    if (!home.empty()) {
        return home + "/Library/Logs/VertexNova";
    }

#elif defined(VNE_PLATFORM_IOS)
    // iOS: Use ~/Documents (app sandbox)
    if (!home.empty()) {
        return home + "/Documents/VertexNova/logs";
    }

#elif defined(VNE_PLATFORM_ANDROID)
    // Android: App internal storage
    return "/data/data/com.vertexnova.app/files/logs";

#elif defined(VNE_PLATFORM_LINUX)
    // Linux: XDG Base Directory Specification
    if (const char* xdg = std::getenv("XDG_DATA_HOME")) {
        return std::string(xdg) + "/VertexNova/logs";
    }
    if (!home.empty()) {
        return home + "/.local/share/VertexNova/logs";
    }

#elif defined(VNE_PLATFORM_WEB)
    // Web: No file logging
    return "";

#endif

    // Fallback: Check if we're in a build directory
    std::string cwd = getCurrentDirectory();
    if (isBuildDirectory(cwd)) {
        return cwd + sep + "logs";
    }

    // Final fallback
    return "logs";
}

bool Logging::ensureLogDirectoryExists(const std::string& log_dir) {
    if (log_dir.empty()) {
        return false;
    }
    return createDirectories(log_dir);
}

std::string Logging::createLoggingFolder(const std::string& base_dir, const std::string& filename) {
    const std::string sep = getPathSeparator();

    // Generate timestamp folder name
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo{};

#if defined(VNE_PLATFORM_WINDOWS)
    localtime_s(&timeinfo, &time);
#else
    localtime_r(&time, &timeinfo);
#endif

    std::ostringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
    std::string timestamped_dir = base_dir + sep + ss.str();

    // Try to create timestamped directory
    if (createDirectories(timestamped_dir)) {
        return timestamped_dir + sep + filename;
    }

    // Fallback to base directory
    if (createDirectories(base_dir)) {
        return base_dir + sep + filename;
    }

    // Final fallback
    return filename;
}

}  // namespace vne::log
