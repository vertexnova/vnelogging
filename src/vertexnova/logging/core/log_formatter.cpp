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

#include "log_formatter.h"

#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>

namespace vne {  // Outer namespace
namespace log {  // Inner namespace
std::string LogFormatter::getThreadID() {
    // Use thread_local to cache the thread ID string per thread - no mutex needed for reads
    thread_local std::string s_cached_id;

    if (s_cached_id.empty()) {
        // Only lock when assigning a new ID (once per thread)
        static std::mutex s_id_mutex;
        static int s_next_id = 1;

        std::lock_guard<std::mutex> lock(s_id_mutex);
        s_cached_id = "Thread-" + std::to_string(s_next_id++);
    }

    return s_cached_id;
}

std::string LogFormatter::format(const std::string& name,
                                 LogLevel level,
                                 TimeStampType time_stamp_type,
                                 const std::string& message,
                                 const std::string& file,
                                 const std::string& function,
                                 uint32_t line,
                                 const std::string& format) {
    std::ostringstream message_stream;
    for (size_t i = 0; i < format.length(); ++i) {
        if (format[i] == '%' && i + 1 < format.length()) {
            switch (format[i + 1]) {
                case 'x': {  // Timestamp
                    TimeStamp time_stamp(time_stamp_type);
                    message_stream << time_stamp.getTimeStamp();
                    i++;  // Skip next character
                } break;
                case 'n':  // Log category
                    message_stream << name;
                    i++;  // Skip next character
                    break;
                case 'l':  // Log level
                    message_stream << level;
                    i++;  // Skip next character
                    break;
                case 't':  // Thread ID
                    message_stream << getThreadID();
                    i++;  // Skip next character
                    break;
                case '$':  // file name
                    message_stream << file;
                    i++;  // Skip the next character
                    break;
                case '!':  // function name
                    message_stream << function;
                    i++;  // Skip the next character
                    break;
                case '#':  // line number
                    message_stream << line;
                    i++;  // Skip the next character
                    break;
                case 'v':
                    message_stream << message;
                    i++;  // Skip the next character
                    break;
                default:
                    message_stream << format[i];
                    break;
            }
        } else {
            message_stream << format[i];
        }
    }
    return message_stream.str();
}

}  // namespace log
}  // namespace vne
