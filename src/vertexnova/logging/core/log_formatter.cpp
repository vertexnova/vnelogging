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
    static std::mutex mtx;
    static std::map<std::thread::id, std::string> thread_ids;
    static int next_id = 1;

    std::lock_guard<std::mutex> lock(mtx);
    auto this_id = std::this_thread::get_id();

    auto it = thread_ids.find(this_id);
    if (it != thread_ids.end()) {
        return it->second;
    } else {
        std::ostringstream oss;
        oss << "Thread-" << next_id++;
        thread_ids[this_id] = oss.str();
        return oss.str();
    }
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
