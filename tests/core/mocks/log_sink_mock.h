#ifndef LOG_SINK_MOCK_H
#define LOG_SINK_MOCK_H

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

#include "vertexnova/logging/core/log_sink.h"

#include <gmock/gmock.h>

namespace vne {
namespace log {
/**
 * @file log_sink_mock.h
 *
 * @brief Mock implementation of the ILogSink interface for unit testing.
 *
 * This mock class is used to simulate behavior of log sinks for testing purposes.
 *
 * @see ILogSink
 */

class LogSinkMock : public ILogSink {
   public:
    /**
     * @brief Mock method to log a message.
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
    MOCK_METHOD(void,
                log,
                (const std::string& name,
                 LogLevel level,
                 TimeStampType time_stamp_type,
                 const std::string& message,
                 const std::string& file,
                 const std::string& function,
                 uint32_t line),
                (override));

    /**
     * @brief Mock method to flush the log sink.
     */
    MOCK_METHOD(void, flush, (), (override));

    /**
     * @brief Gets the log pattern (mock implementation).
     *
     * @return An empty string as this is a mock implementation.
     */
    std::string getPattern() const override { return ""; }

    /**
     * @brief Sets the log pattern (mock implementation).
     *
     * @param pattern The new log pattern (not used in mock).
     */
    void setPattern(const std::string& pattern) override {}

    /**
     * @brief Clones the log sink (mock implementation).
     *
     * @return A new instance of LogSinkMock (mock implementation).
     */
    std::unique_ptr<ILogSink> clone() const override { return std::make_unique<LogSinkMock>(); }
};

}  // namespace log
}  // namespace vne

#endif  // LOG_SINK_MOCK_H
