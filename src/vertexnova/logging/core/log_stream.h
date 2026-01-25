#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   June 2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "log_level.h"
#include "time_stamp.h"

#include <string>
#include <sstream>

namespace vne::log {

/**
 * @class LogStream
 * @brief Provides a streaming interface for logging messages.
 *
 * The LogStream class allows logging messages to be streamed and formatted
 * before being sent to the logger. It captures log details such as message category,
 * severity level, file name, function name, and line number.
 */
class LogStream {
   public:
    /**
     * @brief Constructs a LogStream object with a specific logger name.
     *
     * This constructor allows specifying a specific logger to use for the log message
     * rather than the default logger.
     *
     * @param logger_name The name of the logger to use.
     * @param category The category name for the log message.
     * @param level The log level.
     * @param time_stamp_type The type of timestamp to generate.
     * @param file The name of the source file where the log was generated.
     * @param function The function from which the log is called.
     * @param line The line number in the source file where the log was generated.
     */
    LogStream(const char* logger_name,
              std::string category,
              LogLevel level,
              TimeStampType time_stamp_type,
              std::string file,
              std::string function,
              uint32_t line);

    /**
     * @brief Destructor for LogStream.
     *
     * The destructor sends the accumulated log message to the logger.
     */
    ~LogStream();

    /**
     * @brief Overloaded stream insertion operator.
     *
     * This template method allows any type of message to be streamed into the log.
     *
     * @tparam T The type of the message to be logged.
     * @param msg The message to be logged.
     * @return Reference to the LogStream object to allow chaining.
     */
    template<typename T>
    LogStream& operator<<(const T& msg) {
        msg_stream_ << msg;
        return *this;
    }

   private:
    const char* logger_name_;        //!< The name of the logger to use
    std::string category_;           //!< Log message category name (metadata for the log message)
    LogLevel log_level_;             //!< The severity level of the log message.
    TimeStampType time_stamp_type_;  //!< The type of timestamp to generate.
    std::string file_;               //!< The name of file where log is generated
    std::string function_;           //!< The name of the function where the log is generated.
    uint32_t line_;                  //!< The line number in the source code where the log is generated.
    std::stringstream msg_stream_;   //!< Internal stream to accumulate the log message.
};

}  // namespace vne::log
