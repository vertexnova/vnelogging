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

namespace vne::log {

/**
 * @class LogFormatter
 * @brief A class for formatting log messages.
 */
class LogFormatter {
   public:
    /**
     * @brief Formats a log message according to the specified format.
     *
     * @param name The category name for the log message.
     * @param level The log level.
     * @param time_stamp_type The type of timestamp to generate. This specifies whether the timestamp should be in local
     * time or UTC.
     * @param message The log message.
     * @param file The name of the source file where the log was generated.
     * @param function The function from which the log is called.
     * @param line The line number in the source file where the log was generated.
     * @param format The format string for the log message. Defaults to "%x [%l] [%n] :: %v : [%!], [%#]".
     * @return The formatted log message.
     */
    static std::string format(const std::string& name,
                              LogLevel level,
                              TimeStampType time_stamp_type,
                              const std::string& message,
                              const std::string& file,
                              const std::string& function,
                              uint32_t line,
                              const std::string& format = "%x [%l] [%n] :: %v : [%!], [%#]");

   private:
    /**
     * @brief Retrieves the thread ID in a formatted string.
     *
     * @return The formatted thread ID string.
     */
    static std::string getThreadID();
};

}  // namespace vne::log
