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

#include <string>

namespace vne::log {

/**
 * @class ConsoleLogSink
 * @brief Concrete implementation of ILogSink for console logging.
 *
 * This class provides a console-based logging implementation. It
 * outputs log messages to the standard console.
 */
class ConsoleLogSink : public ILogSink {
   public:
    /**
     * @brief Constructor.
     *
     * Initializes the console logger with an optional log pattern.
     * Defaul pattern is "%x [%l] %v".
     */
    ConsoleLogSink();

    /**
     * @brief Destructor.
     *
     * Cleans up resources used by the console logger.
     */
    ~ConsoleLogSink();

    // Deleted copy constructor and assignment operator
    ConsoleLogSink(const ConsoleLogSink&) = delete;
    ConsoleLogSink& operator=(const ConsoleLogSink&) = delete;

   public:
    /**
     * @brief Logs a message to the console.
     *
     * Overrides the log method from ILogSink to output messages to
     * the standard console.
     *
     * @param name The category name for the log message.
     * @param level The log level of the message.
     * @param time_stamp_type The type of timestamp to generate.
     *                        This specifies whether the timestamp should be in local time or UTC.
     * @param message The message content to log.
     * @param file The file name where the log was generated.
     * @param function The function name where the log was generated.
     * @param line The line number where the log was generated.
     */
    void log(const std::string& name,
             LogLevel level,
             TimeStampType time_stamp_type,
             const std::string& message,
             const std::string& file,
             const std::string& function,
             uint32_t line) override;

    /**
     * @brief Flushes the console output.
     *
     * Ensures that all log messages have been output to the console.
     */
    void flush() override;

    /**
     * @brief Gets the current log pattern.
     *
     * @return The current log pattern.
     */
    std::string getPattern() const override;

    /**
     * @brief Sets a new log pattern.
     *
     * @param pattern The new log pattern.
     */
    void setPattern(const std::string& pattern) override;

    /**
     * @brief Creates a new instance of the console log sink.
     *
     * This factory method creates a new instance of the console log sink.
     *
     * @return A unique pointer to the cloned console log sink instance.
     */
    std::unique_ptr<ILogSink> clone() const override;

   private:
    std::string pattern_;  //!< The log pattern used by the console logger.
};

}  // namespace vne::log
