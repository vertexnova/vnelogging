#pragma once
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

#include "vertexnova/logging/log_manager.h"
#include "vertexnova/logging/core/logger.h"
#include "vertexnova/logging/core/log_stream.h"
#include "vertexnova/logging/core/log_level.h"
#include "vertexnova/logging/core/time_stamp.h"

#include <string>
#include <memory>

namespace vne::log {

/**
 * @enum LogSinkType
 * @brief Specifies the type of log sinks available.
 *
 * This enum class defines the different types of log sinks that can be used
 * in the logging system. A log sink determines where the log messages will be
 * output.
 */
enum class LogSinkType {
    eNone = 0,                //!< No logs will be output.
    eConsole = 1 << 0,        //!< Logs will be output to the console.
    eFile = 1 << 1,           //!< Logs will be written to a file.
    eBoth = eConsole | eFile  //!< Logs will be output to both console and file.
};

/**
 * @struct LoggerConfig
 * @brief Configuration structure for setting up a logger.
 *
 * This structure holds the configuration parameters for a logger,
 * including its name, sink type, patterns for console and file output,
 * log level, and whether it operates asynchronously.
 */
struct LoggerConfig {
    std::string name;                          //!< The name of the logger.
    LogSinkType sink = LogSinkType::eConsole;  //!< The type of log sink to use (console, file, or both).
    std::string console_pattern;               //!< The pattern for formatting log messages in the console.
    std::string file_pattern;                  //!< The pattern for formatting log messages in a file.
    std::string file_path;                     //!< The path to the file where logs will be written, if applicable.
    LogLevel log_level = LogLevel::eInfo;      //!< The log level for filtering messages (e.g., trace, debug, info,
                                               //!< warn, error, fatal).
    LogLevel flush_level = LogLevel::eError;   //!< The log level at which the logger will flush its output.
    bool async = false;                        //!< Flag indicating whether the logger operates asynchronously.
};

extern const char* kDefaultLoggerName;  //!< Default logger name used in macros and log streams.

/**
 * @class Logging
 * @brief Provides setup functionality for the logging system.
 *
 * This class handles the setup and configuration of the logging system,
 * including initialization, pattern setup, sink configuration, and macros
 * for logging operations.
 */
class Logging {
   public:
    /**
     * @brief Initializes the logging system with the specified logger name.
     *
     * This function sets up the logging system for the application. The logger can be configured
     * to operate in either synchronous or asynchronous mode.
     *
     * @param logger_name The name to assign to the logger instance.
     * @param async If true, enables asynchronous logging; otherwise, logging is synchronous. Default is false.
     */
    static void initialize(const std::string& logger_name, bool async = false);

    /**
     * @brief Shuts down the logging system.
     *
     * Cleans up and shuts down the logging system, releasing resources
     * and closing any open log files.
     */
    static void shutdown();

    /**
     * @brief Retrieves the LogManager instance.
     *
     * Provides access to the singleton instance of the LogManager
     * used for managing logging operations.
     *
     * @return A reference to the shared pointer holding the LogManager instance.
     */
    static std::shared_ptr<LogManager>& getLogManager() { return log_manager_; }

    /**
     * @brief Retrieves a logger by name.
     *
     * This function returns a shared pointer to the logger instance
     * associated with the given name. If the logger does not exist,
     * a null pointer is returned.
     *
     * @param name The name of the logger to retrieve.
     * @return A shared pointer to the logger instance, or null if not found.
     */
    static std::shared_ptr<ILogger> getLogger(const std::string& name) { return log_manager_->getLogger(name); }

    /**
     * @brief Checks if a specific logger is configured for asynchronous operation.
     *
     * This function returns whether the specified logger is configured for async logging.
     *
     * @param logger_name The name of the logger to check.
     * @return true if the logger is async, false if it's sync or doesn't exist.
     */
    static bool isLoggerAsync(const std::string& logger_name);

    /**
     * @brief Adds a console sink to the logger.
     *
     * This function configures the specified logger to output log messages
     * to the console.
     *
     * @param logger_name The name of the logger to which the console sink will be added.
     */
    static void addConsoleSink(const std::string& logger_name);

    /**
     * @brief Adds a file sink to the logger.
     *
     * This function configures the specified logger to write log messages
     * to the given file.
     *
     * @param logger_name The name of the logger to which the file sink will be added.
     * @param file The name of the file where logs will be written.
     */
    static void addFileSink(const std::string& logger_name, const std::string& file);

    /**
     * @brief Sets the console pattern for the logger.
     *
     * This function configures the specified logger to use the given pattern
     * for formatting log messages output to the console.
     *
     * @param logger_name The name of the logger for which to set the console pattern.
     * @param pattern The pattern to be used for formatting console log messages.
     */
    static void setConsolePattern(const std::string& logger_name, const std::string& pattern);

    /**
     * @brief Sets the file pattern for the logger.
     *
     * This function configures the specified logger to use the given pattern
     * for formatting log messages written to a file.
     *
     * @param logger_name The name of the logger for which to set the file pattern.
     * @param pattern The pattern to be used for formatting file log messages.
     */
    static void setFilePattern(const std::string& logger_name, const std::string& pattern);

    /**
     * @brief Sets the log level for the logger.
     *
     * This function configures the specified logger to use the given log level.
     *
     * @param logger_name The name of the logger for which to set the log level.
     * @param level The log level to be set for the logger.
     */
    static void setLogLevel(const std::string& logger_name, LogLevel level);

    /**
     * @brief Sets the flush level for the logger.
     *
     * This function configures the specified logger to use the given flush level.
     *
     * @param logger_name The name of the logger for which to set the flush level.
     * @param level The flush level to be set for the logger.
     */
    static void setFlushLevel(const std::string& logger_name, LogLevel level);

    /**
     * @brief Gets the appropriate log directory based on build context.
     *
     * Determines the best location for log files based on:
     * 1. Build directory (if available)
     * 2. Current working directory (if it looks like a build dir)
     * 3. Fallback to "logfiles" in project root
     *
     * @return The path to the log directory.
     */
    static std::string getLogDirectory();

    /**
     * @brief Gets platform-specific log directory with enhanced mobile support.
     *
     * Provides platform-aware log directory paths following best practices:
     * - Windows: AppData\Local\VertexNova\logs
     * - macOS: ~/Library/Logs/VertexNova
     * - Linux: ~/.local/share/VertexNova/logs (XDG compliant)
     * - Android: /data/data/com.vertexnova.app/files/logs (app-private)
     * - iOS: ~/Documents/VertexNova/logs (app-private, backed up)
     *
     * @return The platform-specific path to the log directory.
     */
    static std::string getPlatformSpecificLogDirectory();

    /**
     * @brief Ensures the log directory exists, creating it if necessary.
     *
     * Creates the specified log directory and all parent directories if they don't exist.
     * Provides error handling and logging for directory creation failures.
     *
     * @param log_dir The path to the log directory to ensure exists.
     * @return True if the directory exists or was successfully created, false otherwise.
     */
    static bool ensureLogDirectoryExists(const std::string& log_dir);

    /**
     * @brief Creates a logging folder.
     *
     * Creates a folder for storing log files within the specified base
     * directory and returns the full path to the created log file.
     *
     * @param base_dir The base directory for the logging folder.
     * @param filename The name of the log file.
     * @return The full path to the created log file.
     */
    static std::string createLoggingFolder(const std::string& base_dir, const std::string& filename);

    /**
     * @brief Returns a default logger configuration.
     *
     * This function provides a default configuration for a logger, including
     * default name, sink, patterns, file path, log level, and async flag.
     *
     * @return A LoggerConfig struct with default values.
     */
    static LoggerConfig defaultLoggerConfig();

    /**
     * @brief Configures a logger in a single step.
     *
     * This function sets up a logger according to the provided configuration struct.
     * It creates the logger if it does not exist, adds the specified sinks, sets
     * patterns only for the enabled sinks, and sets the log level and file path.
     *
     * @param cfg The logger configuration struct specifying all options.
     */
    static void configureLogger(const LoggerConfig& cfg);

   private:
    static std::shared_ptr<LogManager> log_manager_;  //!< The LogManager instance for managing logging operations.
};

}  // namespace vne::log

// Macros for creating and using loggers

/**
 * @def CREATE_VNE_LOGGER_CATEGORY(name)
 * @brief Macro to create a constant logger category.
 *
 * This macro creates a constant `_logger_category` based on the provided `name`.
 * The category is used as metadata for log messages and helps to organize and filter logs.
 * It is completely separate from the logger name, which determines which configured
 * logger (with its settings) will handle the message.
 *
 * @param name The name of the logger category.
 */
#define CREATE_VNE_LOGGER_CATEGORY(name) constexpr const char* _logger_category = name;

/**
 * @def __FUNCTION_NAME__
 * @brief Macro to get the current function name.
 *
 * This macro provides a standard way to retrieve the name of the current
 * function. It uses different compiler-specific macros depending on the
 * platform.
 */
#ifndef __FUNCTION_NAME__
#ifdef WIN32  // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else  //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

/**
 * @def _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, LEVEL)
 * @brief General logging preprocessor macro with explicit logger.
 *
 * This macro provides a convenient way to log messages at different severity
 * levels to a specific logger. It uses the vne::log::LogStream class to format
 * and output log messages.
 *
 * @param LOGGER The name of the logger to use.
 * @param CATEGORY The category for the log message.
 * @param LEVEL The severity level to log at.
 */
#define _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, LEVEL) \
    ::vne::log::LogStream(LOGGER, CATEGORY, LEVEL, ::vne::log::TimeStampType::eLocal, __FILE__, __FUNCTION_NAME__, __LINE__)

#define _VNE_LOG_TRACE_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eTrace)
#define _VNE_LOG_DEBUG_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eDebug)
#define _VNE_LOG_INFO_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eInfo)
#define _VNE_LOG_WARN_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eWarn)
#define _VNE_LOG_ERROR_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eError)
#define _VNE_LOG_FATAL_LC(LOGGER, CATEGORY) _VNE_LOG_WITH_LOGGER(LOGGER, CATEGORY, ::vne::log::LogLevel::eFatal)

#define _VNE_LOG_TRACE_L(LOGGER) _VNE_LOG_TRACE_LC(LOGGER, _logger_category)
#define _VNE_LOG_DEBUG_L(LOGGER) _VNE_LOG_DEBUG_LC(LOGGER, _logger_category)
#define _VNE_LOG_INFO_L(LOGGER) _VNE_LOG_INFO_LC(LOGGER, _logger_category)
#define _VNE_LOG_WARN_L(LOGGER) _VNE_LOG_WARN_LC(LOGGER, _logger_category)
#define _VNE_LOG_ERROR_L(LOGGER) _VNE_LOG_ERROR_LC(LOGGER, _logger_category)
#define _VNE_LOG_FATAL_L(LOGGER) _VNE_LOG_FATAL_LC(LOGGER, _logger_category)

#define VNE_LOG_TRACE _VNE_LOG_TRACE_L(::vne::log::kDefaultLoggerName)
#define VNE_LOG_DEBUG _VNE_LOG_DEBUG_L(::vne::log::kDefaultLoggerName)
#define VNE_LOG_INFO _VNE_LOG_INFO_L(::vne::log::kDefaultLoggerName)
#define VNE_LOG_WARN _VNE_LOG_WARN_L(::vne::log::kDefaultLoggerName)
#define VNE_LOG_ERROR _VNE_LOG_ERROR_L(::vne::log::kDefaultLoggerName)
#define VNE_LOG_FATAL _VNE_LOG_FATAL_L(::vne::log::kDefaultLoggerName)
