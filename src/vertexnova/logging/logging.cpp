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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef VNE_PLATFORM_IOS
#include <unistd.h>
#include <limits.h>
#else
#include <filesystem>
#endif
#include <ctime>
#include <exception>
#include "core/log_level.h"
#include "core/time_stamp.h"

namespace vne {
namespace log {

const char* kDefaultLoggerName = "vertexnova";  // Definition of the default logger name
std::shared_ptr<LogManager> Logging::log_manager_ = nullptr;

void Logging::initialize(const std::string& name, bool async) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->createLogger(name, async);
}

void Logging::shutdown() {
    if (log_manager_) {
        log_manager_->finalize();
        log_manager_.reset();
    }
}

bool Logging::isLoggerAsync(const std::string& logger_name) {
    if (!log_manager_) {
        return false;  // If log manager is not initialized, assume sync
    }
    return log_manager_->isLoggerAsync(logger_name);
}

void Logging::addConsoleSink(const std::string& logger_name) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->addConsoleSink(logger_name);
}

void Logging::addFileSink(const std::string& logger_name, const std::string& file) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->addFileSink(logger_name, file);
}

void Logging::setConsolePattern(const std::string& logger_name, const std::string& pattern) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->setConsolePattern(logger_name, pattern);
}

void Logging::setFilePattern(const std::string& logger_name, const std::string& pattern) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->setFilePattern(logger_name, pattern);
}

void Logging::setLogLevel(const std::string& logger_name, LogLevel level) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->setLogLevel(logger_name, level);
}

void Logging::setFlushLevel(const std::string& logger_name, LogLevel level) {
    if (!log_manager_) {
        log_manager_ = std::make_shared<LogManager>();
    }
    log_manager_->setFlushLevel(logger_name, level);
}

LoggerConfig Logging::defaultLoggerConfig() {
    LoggerConfig config;
    config.name = kDefaultLoggerName;  // Use the default logger name
    config.console_pattern = "%x [%l] %v";
    config.file_pattern = "%x [%n] [%l] [%!] %v";

#ifdef VNE_PLATFORM_WEB
    // Web platform: console-only logging
    config.sink = LogSinkType::eConsole;
    config.file_path = "";  // No file path for web
    config.log_level = LogLevel::eInfo;
    config.flush_level = LogLevel::eError;
    config.async = false;  // Synchronous logging for web
#else
    // Desktop platforms: use platform-specific configuration
    config.sink = LogSinkType::eConsole;

    // Use platform-specific log directory detection
    std::string log_dir = getPlatformSpecificLogDirectory();
    config.file_path = log_dir + "/vne.log";

    // Ensure the log directory exists
    ensureLogDirectoryExists(log_dir);

    config.log_level = LogLevel::eInfo;
    config.flush_level = LogLevel::eError;
    config.async = false;
#endif

    return config;
}

void Logging::configureLogger(const LoggerConfig& cfg) {
    // Ensure the log manager is initialized and create the logger with the specified async setting
    initialize(cfg.name, cfg.async);

    // Configure the logger with the rest of the settings
    if (cfg.sink == LogSinkType::eConsole || cfg.sink == LogSinkType::eBoth) {
        addConsoleSink(cfg.name);
        if (!cfg.console_pattern.empty()) {
            setConsolePattern(cfg.name, cfg.console_pattern);
        }
    }

#ifdef VNE_PLATFORM_WEB
    // Web platform: ignore file sink configuration
    if (cfg.sink == LogSinkType::eFile || cfg.sink == LogSinkType::eBoth) {
        // Force console-only logging on web platform
        addConsoleSink(cfg.name);
        if (!cfg.console_pattern.empty()) {
            setConsolePattern(cfg.name, cfg.console_pattern);
        }
    }
#else
    // Desktop platforms: allow file sink configuration
    if (cfg.sink == LogSinkType::eFile || cfg.sink == LogSinkType::eBoth) {
        addFileSink(cfg.name, cfg.file_path);
        if (!cfg.file_pattern.empty()) {
            setFilePattern(cfg.name, cfg.file_pattern);
        }
    }
#endif

    setLogLevel(cfg.name, cfg.log_level);
    setFlushLevel(cfg.name, cfg.flush_level);
}

std::string Logging::getLogDirectory() {
    // Platform-specific log directory detection following best practices

#ifdef VNE_PLATFORM_WIN
    // Windows: Use AppData\Local for logs (persistent, user-specific)
    const char* appdata = std::getenv("LOCALAPPDATA");
    if (appdata) {
        return std::string(appdata) + "\\VertexNova\\logs";
    }
    // Fallback to APPDATA if LOCALAPPDATA is not available
    const char* roaming_appdata = std::getenv("APPDATA");
    if (roaming_appdata) {
        return std::string(roaming_appdata) + "\\VertexNova\\logs";
    }
    // Final fallback to user profile
    const char* userprofile = std::getenv("USERPROFILE");
    if (userprofile) {
        return std::string(userprofile) + "\\AppData\\Local\\VertexNova\\logs";
    }

#elif defined(VNE_PLATFORM_MAC) || defined(VNE_PLATFORM_IOS)
    // macOS/iOS: Use ~/Library/Logs for logs (standard location)
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Logs/VertexNova";
    }

#elif defined(VNE_PLATFORM_ANDROID)
    // Android: Use app's internal storage for logs
    // This will be handled by the Android-specific implementation
    // For now, return a placeholder that will be overridden
    return "/data/data/com.vertexnova.app/logs";

#elif defined(VNE_PLATFORM_LINUX)
    // Linux: Use XDG Base Directory Specification
    // Priority: XDG_DATA_HOME, then ~/.local/share
    const char* xdg_data_home = std::getenv("XDG_DATA_HOME");
    if (xdg_data_home) {
        return std::string(xdg_data_home) + "/VertexNova/logs";
    }

    // Fallback to ~/.local/share
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.local/share/VertexNova/logs";
    }

#else
    // Generic Unix-like systems
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.vertexnova/logs";
    }
#endif

    // Development/fallback paths
#ifdef VNE_BUILD_DIR
    return std::string(VNE_BUILD_DIR) + "/logs";
#else
    // Try to detect if we're in a build directory
#ifdef VNE_PLATFORM_IOS
    // iOS: use basic string operations instead of std::filesystem
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::string current_dir_str(cwd);

        // Check if current directory looks like a build directory
        if (current_dir_str.find("build") != std::string::npos || current_dir_str.find("out") != std::string::npos
            || current_dir_str.find("bin") != std::string::npos) {
            return current_dir_str + "/logs";
        }
    }
#else
    // Use std::filesystem for platforms that support it
    std::filesystem::path current_dir = std::filesystem::current_path();
    std::string current_dir_str = current_dir.string();

    // Check if current directory looks like a build directory
    if (current_dir_str.find("build") != std::string::npos || current_dir_str.find("out") != std::string::npos
        || current_dir_str.find("bin") != std::string::npos) {
        return current_dir_str + "/logs";
    }
#endif

    // Final fallback for development
    return "logfiles";
#endif
}

std::string Logging::getPlatformSpecificLogDirectory() {
    // Enhanced platform-specific log directory with better mobile support

#ifdef VNE_PLATFORM_WEB
    // Web platform: no file logging, return empty string
    return "";
#elif defined(VNE_PLATFORM_ANDROID)
    // Android: Use app's internal storage (private to the app)
    // In a real Android app, you would use JNI to get the actual app directory
    // For now, we'll use a placeholder that should be replaced with actual Android API calls
    return "/data/data/com.vertexnova.app/files/logs";

#elif defined(VNE_PLATFORM_IOS)
    // iOS: Use app's Documents directory (persistent, backed up)
    // In a real iOS app, you would use Objective-C/Swift to get the actual path
    // For now, we'll use a placeholder that should be replaced with actual iOS API calls
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/Documents/VertexNova/logs";
    }

#elif defined(VNE_PLATFORM_WIN)
    // Windows: Use AppData\Local for logs (persistent, user-specific)
    const char* appdata = std::getenv("LOCALAPPDATA");
    if (appdata) {
        return std::string(appdata) + "\\VertexNova\\logs";
    }
    // Fallback to APPDATA if LOCALAPPDATA is not available
    const char* roaming_appdata = std::getenv("APPDATA");
    if (roaming_appdata) {
        return std::string(roaming_appdata) + "\\VertexNova\\logs";
    }
    // Final fallback to user profile
    const char* userprofile = std::getenv("USERPROFILE");
    if (userprofile) {
        return std::string(userprofile) + "\\AppData\\Local\\VertexNova\\logs";
    }

#elif defined(VNE_PLATFORM_MAC)
    // macOS: Use ~/Library/Logs for logs (standard location)
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Logs/VertexNova";
    }

#elif defined(VNE_PLATFORM_LINUX)
    // Linux: Use XDG Base Directory Specification
    // Priority: XDG_DATA_HOME, then ~/.local/share
    const char* xdg_data_home = std::getenv("XDG_DATA_HOME");
    if (xdg_data_home) {
        return std::string(xdg_data_home) + "/VertexNova/logs";
    }

    // Fallback to ~/.local/share
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.local/share/VertexNova/logs";
    }

#else
    // Generic Unix-like systems
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.vertexnova/logs";
    }
#endif

    // Development/fallback paths
#ifdef VNE_BUILD_DIR
    return std::string(VNE_BUILD_DIR) + "/logs";
#else
    // Try to detect if we're in a build directory
#ifdef VNE_PLATFORM_IOS
    // iOS: use basic string operations instead of std::filesystem
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::string current_dir_str(cwd);

        // Check if current directory looks like a build directory
        if (current_dir_str.find("build") != std::string::npos || current_dir_str.find("out") != std::string::npos
            || current_dir_str.find("bin") != std::string::npos) {
            return current_dir_str + "/logs";
        }
    }
#else
    std::filesystem::path current_dir = std::filesystem::current_path();
    std::string current_dir_str = current_dir.string();

    // Check if current directory looks like a build directory
    if (current_dir_str.find("build") != std::string::npos || current_dir_str.find("out") != std::string::npos
        || current_dir_str.find("bin") != std::string::npos) {
        return current_dir_str + "/logs";
    }
#endif

    // Final fallback for development
    return "logfiles";
#endif
}

bool Logging::ensureLogDirectoryExists(const std::string& log_dir) {
    try {
#ifdef VNE_PLATFORM_IOS
        // iOS: use basic file operations
        // For iOS, we'll assume the directory exists or create it with basic operations
        // In a real iOS app, you would use NSFileManager to handle this properly
        return true;  // Simplified for iOS compatibility
#else
        if (!std::filesystem::exists(log_dir)) {
            std::error_code ec;
            bool created = std::filesystem::create_directories(log_dir, ec);
            if (!created && ec) {
                // Use logging system instead of std::cout
                if (log_manager_) {
                    auto logger = log_manager_->getLogger(kDefaultLoggerName);
                    if (logger) {
                        logger->log(kDefaultLoggerName,
                                    LogLevel::eWarn,
                                    TimeStampType::eLocal,
                                    "Failed to create log directory: " + log_dir + " - Error: " + ec.message(),
                                    __FILE__,
                                    __FUNCTION__,
                                    __LINE__);
                    }
                }
                return false;
            }
        }
        return true;
#endif
    } catch (const std::exception& e) {
        // Use logging system instead of std::cout
        if (log_manager_) {
            auto logger = log_manager_->getLogger(kDefaultLoggerName);
            if (logger) {
                logger->log(kDefaultLoggerName,
                            LogLevel::eError,
                            TimeStampType::eLocal,
                            "Exception in ensureLogDirectoryExists: " + std::string(e.what()),
                            __FILE__,
                            __FUNCTION__,
                            __LINE__);
            }
        }
        return false;
    }
}

std::string Logging::createLoggingFolder(const std::string& base_dir, const std::string& filename) {
    try {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;

#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&timeinfo, &time);
        std::string slash = "\\";
#else
        localtime_r(&time, &timeinfo);
        std::string slash = "/";
#endif

        std::stringstream ss;
        ss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
        std::string log_path = base_dir + slash + ss.str();

        // Try to create the directory with error handling
#ifdef VNE_PLATFORM_IOS
        // iOS: simplified directory handling
        // In a real iOS app, you would use NSFileManager to handle this properly
        // For now, we'll just use the base directory
        log_path = base_dir;
#else
        if (!std::filesystem::exists(log_path)) {
            std::error_code ec;
            bool created = std::filesystem::create_directories(log_path, ec);
            if (!created && ec) {
                // If timestamped directory creation fails, fall back to base directory
                // Use logging system instead of std::cout
                if (log_manager_) {
                    auto logger = log_manager_->getLogger(kDefaultLoggerName);
                    if (logger) {
                        logger->log(kDefaultLoggerName,
                                    LogLevel::eWarn,
                                    TimeStampType::eLocal,
                                    "Failed to create timestamped log directory: " + log_path + " - Error: " + ec.message(),
                                    __FILE__,
                                    __FUNCTION__,
                                    __LINE__);
                    }
                }
                log_path = base_dir;
            }
        }
#endif

        std::string log_file_path = log_path + slash + filename;
        return log_file_path;
    } catch (const std::exception& e) {
        // If anything fails, return a simple fallback path
        // Use logging system instead of std::cout
        if (log_manager_) {
            auto logger = log_manager_->getLogger(kDefaultLoggerName);
            if (logger) {
                logger->log(kDefaultLoggerName,
                            LogLevel::eError,
                            TimeStampType::eLocal,
                            "Exception in createLoggingFolder: " + std::string(e.what()),
                            __FILE__,
                            __FUNCTION__,
                            __LINE__);
            }
        }
        return base_dir + "/vne.log";
    }
}

}  // namespace log
}  // namespace vne
