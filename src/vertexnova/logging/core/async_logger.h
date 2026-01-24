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

#include "logger.h"
#include "log_dispatcher.h"

#include <memory>
#include <string>
#include <vector>

namespace vne::log {

/**
 * @class AsyncLogger
 * @brief Asynchronous logger implementation using LogDispatcher, LogWorkerQueue, and LogQueue.
 *
 * This logger asynchronously dispatches log messages to a worker queue for processing.
 * It manages log sinks, log levels, and provides methods to log messages and flush logs.
 */
class AsyncLogger : public ILogger {
   public:
    /**
     * @brief Constructor.
     *
     * @param logger_name The name of the logger.
     */
    explicit AsyncLogger(std::string logger_name);

    /**
     * @brief Destructor.
     *
     * Cleans up resources and stops the worker queue.
     */
    ~AsyncLogger() override;

    /**
     * @brief Adds a log sink to the logger.
     *
     * @param log_sink A unique pointer to a log sink.
     */
    void addLogSink(std::unique_ptr<ILogSink> log_sink) override;

    /**
     * @brief Retrieves the list of log sinks.
     *
     * @return A reference to the vector of unique pointers to log sinks.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<ILogSink>>& getLogSinks() const override;

    /**
     * @brief Sets the current log level.
     *
     * @param level The log level.
     */
    void setCurrentLogLevel(LogLevel level) override;

    /**
     * @brief Returns the current log level.
     *
     * @return The current log level.
     */
    [[nodiscard]] LogLevel getCurrentLogLevel() const override;

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
    void log(const std::string& category_name,
             LogLevel level,
             TimeStampType time_stamp_type,
             const std::string& message,
             const std::string& file,
             const std::string& function,
             uint32_t line) override;

    /**
     * @brief Flushes all loggers.
     */
    void flush() override;

    /**
     * @brief Retrieves the name of the logger.
     *
     * @return The name of the logger.
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @brief Factory method to create a new instance of the logger with a logger name.
     *
     * @param logger_name The name for the new logger instance.
     * @return A unique pointer to the cloned logger instance.
     */
    [[nodiscard]] std::unique_ptr<ILogger> clone(const std::string& logger_name) const override;

    /**
     * @brief Sets the flush level.
     *
     * @param level The flush level.
     */
    void setFlushLevel(LogLevel level) override;

    /**
     * @brief Returns the flush level.
     *
     * @return The flush level.
     */
    [[nodiscard]] LogLevel getFlushLevel() const override;

   private:
    std::string logger_name_;                           //!< Name of the logger.
    LogLevel current_log_level_;                        //!< Current log level.
    LogLevel flush_level_ = LogLevel::eError;           //!< Flush level (default: ERROR).
    std::vector<std::unique_ptr<ILogSink>> log_sinks_;  //!< Collection of sinks.
    std::unique_ptr<LogDispatcher> dispatcher_;         //!< The log dispatcher instance.
};

}  // namespace vne::log
