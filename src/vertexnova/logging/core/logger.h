#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   MAY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "log_sink.h"

#include <memory>
#include <string>
#include <vector>

namespace vne::log {

/**
 * @class ILogger
 * @brief Abstract base class for loggers.
 *
 * This class provides the interface for logger implementations, including
 * methods to add log sinks, set and get the log level, log messages, flush log output
 * and get logger names.
 */
class ILogger {
   public:
    virtual ~ILogger() = default;

    /**
     * @brief Adds a sink to the logger.
     *
     * @param log_sink A unique pointer to a log sink.
     */
    virtual void addLogSink(std::unique_ptr<ILogSink> log_sink) = 0;

    /**
     * @brief Retrieves the list of log sinks.
     *
     * @return A reference to the vector of unique pointers to log sinks.
     */
    virtual const std::vector<std::unique_ptr<ILogSink>>& getLogSinks() const = 0;

    /**
     * @brief Sets the current log level.
     *
     * @param level The log level.
     */
    virtual void setCurrentLogLevel(LogLevel level) = 0;

    /**
     * @brief Returns the current log level.
     *
     * @return The current log level.
     */
    virtual LogLevel getCurrentLogLevel() const = 0;

    /**
     * @brief Logs a message.
     *
     * @param category_name The category name for the log message.
     * @param level The log level of the message.
     * @param time_stamp_type The type of timestamp to generate.
     *                        This specifies whether the timestamp should be in local time or UTC.
     * @param message The message content to log.
     * @param file The file name where the log was generated.
     * @param function The function name where the log was generated.
     * @param line The line number where the log was generated.
     */
    virtual void log(const std::string& category_name,
                     LogLevel level,
                     TimeStampType time_stamp_type,
                     const std::string& message,
                     const std::string& file,
                     const std::string& function,
                     uint32_t line) = 0;

    /**
     * @brief Flushes all loggers.
     */
    virtual void flush() = 0;

    /**
     * @brief Retrieves the name of the logger.
     *
     * @return The name of the logger.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Factory method to create a new instance of the logger with a logger name.
     *
     * @param logger_name The name for the new logger instance.
     * @return A unique pointer to the cloned logger instance.
     */
    virtual std::unique_ptr<ILogger> clone(const std::string& logger_name) const = 0;

    /**
     * @brief Sets the flush level.
     *
     * @param level The flush level.
     */
    virtual void setFlushLevel(LogLevel level) = 0;

    /**
     * @brief Returns the flush level.
     *
     * @return The flush level.
     */
    virtual LogLevel getFlushLevel() const = 0;

   protected:
    ILogger() = default;
    ILogger(const ILogger&) = delete;
    ILogger& operator=(const ILogger&) = delete;
};

}  // namespace vne::log
