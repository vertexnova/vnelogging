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

#include "log_level.h"
#include "time_stamp.h"

#include <string>
#include <memory>

namespace vne::log {

/**
 * @class ILogSink
 * @brief Interface for log sinks.
 *
 * This abstract class provides the basic interface that all log sinks
 * must implement. It includes methods for logging messages, flushing
 * the log output, and setting/getting the log pattern.
 */
class ILogSink {
   public:
    /**
     * @brief Destructor.
     *
     * Virtual destructor to allow derived classes to clean up resources.
     */
    virtual ~ILogSink() = default;

    /**
     * @brief Logs a message.
     *
     * This is a pure virtual function that must be implemented by derived classes.
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
    virtual void log(const std::string& name,
                     LogLevel level,
                     TimeStampType time_stamp_type,
                     const std::string& message,
                     const std::string& file,
                     const std::string& function,
                     uint32_t line) = 0;

    /**
     * @brief Flushes the log output.
     *
     * Pure virtual function that must be implemented by derived classes.
     * This function is used to ensure that all log messages have been
     * processed and output.
     */
    virtual void flush() = 0;

    /**
     * @brief Gets the log pattern.
     *
     * Pure virtual function that must be implemented by derived classes.
     * This function returns the current log pattern used by the sink.
     *
     * @return The current log pattern.
     */
    [[nodiscard]] virtual std::string getPattern() const = 0;

    /**
     * @brief Sets the log pattern.
     *
     * Pure virtual function that must be implemented by derived classes.
     * This function sets the log pattern used by the sink.
     *
     * @param pattern The new log pattern.
     */
    virtual void setPattern(const std::string& pattern) = 0;

    /**
     * @brief Factory method to create a new instance of the log sink.
     *
     * @return A unique pointer to the cloned log sink instance.
     */
    [[nodiscard]] virtual std::unique_ptr<ILogSink> clone() const = 0;

   protected:
    /**
     * @brief Default constructor.
     *
     * Protected to prevent direct instantiation of the interface class.
     */
    ILogSink() = default;

    /**
     * @brief Copy constructor.
     *
     * Deleted to prevent copying.
     *
     * @param other The other ILogSink object to copy from.
     */
    ILogSink(const ILogSink&) = delete;

    /**
     * @brief Copy assignment operator.
     *
     * Deleted to prevent assignment.
     *
     * @param other The other ILogSink object to assign from.
     * @return A reference to this object.
     */
    ILogSink& operator=(const ILogSink&) = delete;
};

}  // namespace vne::log
