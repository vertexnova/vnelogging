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

#include "vertexnova/logging/core/logger.h"
#include "vertexnova/logging/core/log_level.h"
#include "vertexnova/logging/core/console_log_sink.h"
#include "vertexnova/logging/core/file_log_sink.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace vne::log {

/**
 * @class LogManager
 * @brief Manages logging configuration and provides logger instances.
 *
 * This class manages logging configuration such as adding sinks (console and file),
 * setting log levels, and providing access to logger instances.
 */
class LogManager final {
   public:
    /**
     * @brief Constructor for LogManager.
     *
     * Initializes the LogManager instance.
     */
    LogManager();

    /**
     * @brief Destructor for LogManager.
     */
    ~LogManager() = default;

    // Disable copy and move operations
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;

    /**
     * @brief Creates or retrieves a logger instance with a specified name.
     *
     * @param logger_name The name of the logger to create or retrieve.
     * @param async Override the default async setting for this specific logger.
     * @return A shared pointer to the created or retrieved logger.
     */
    std::shared_ptr<ILogger> createLogger(const std::string& logger_name, bool async = false);

    /**
     * @brief Retrieves a logger instance by name.
     *
     * @param logger_name The name of the logger to retrieve.
     * @return A shared pointer to the retrieved logger, or nullptr if not found.
     */
    std::shared_ptr<ILogger> getLogger(const std::string& logger_name) const;

    /**
     * @brief Adds a console sink to a logger.
     *
     * This method adds a console sink with the default console pattern to the specified logger.
     * @param logger_name The name of the logger to which the console sink should be added.
     */
    void addConsoleSink(const std::string& logger_name);

    /**
     * @brief Adds a file sink to a logger.
     *
     * @param logger_name The name of the logger to which the file sink should be added.
     * @param log_file_path The path where the log file should be created.
     */
    void addFileSink(const std::string& logger_name, const std::string& log_file_path);

    /**
     * @brief Sets the pattern for the console sink of a logger.
     *
     * @param logger_name The name of the logger for which to set the console pattern.
     * @param pattern The pattern to set for the console sink.
     */
    void setConsolePattern(const std::string& logger_name, const std::string& pattern);

    /**
     * @brief Sets the pattern for the file sink of a logger.
     *
     * @param logger_name The name of the logger for which to set the file pattern.
     * @param pattern The pattern to set for the file sink.
     */
    void setFilePattern(const std::string& logger_name, const std::string& pattern);

    /**
     * @brief Sets the log level for a logger.
     *
     * @param logger_name The name of the logger for which to set the log level.
     * @param level The log level to set.
     */
    void setLogLevel(const std::string& logger_name, LogLevel level);

    /**
     * @brief Sets the flush level for a logger.
     *
     * @param logger_name The name of the logger for which to set the flush level.
     * @param level The flush level to set.
     */
    void setFlushLevel(const std::string& logger_name, LogLevel level);

    /**
     * @brief Checks if a specific logger is configured for asynchronous operation.
     *
     * @param logger_name The name of the logger to check.
     * @return true if the logger is async, false if it's sync or doesn't exist.
     */
    bool isLoggerAsync(const std::string& logger_name) const;

    /**
     * @brief Flushes the logs and unregisters all loggers.
     *
     * This function ensures that all pending log messages are flushed
     * to their respective sinks and then proceeds to unregister all
     * loggers managed by the LogManager instance. This is typically
     * called during the shutdown process to ensure a clean and orderly
     * termination of the logging system.
     */
    void finalize();

   private:
    std::unordered_map<std::string, std::shared_ptr<ILogger>> loggers_;  //!< Registry of logger instances by name.
    std::unordered_map<std::string, bool>
        logger_async_state_;  //!< Tracks which loggers are async (true) vs sync (false).
};

}  // namespace vne::log
