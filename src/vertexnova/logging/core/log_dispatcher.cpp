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

#include "log_dispatcher.h"

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

LogDispatcher::LogDispatcher()
    : log_queue_worker_(log_queue_) {
    log_queue_worker_.start();
}

LogDispatcher::~LogDispatcher() {
    log_queue_worker_.stop();
}

void LogDispatcher::dispatch(const std::vector<std::unique_ptr<ILogSink>>& log_sinks,
                             std::string name,
                             LogLevel level,
                             TimeStampType time_stamp_type,
                             std::string message,
                             std::string file,
                             std::string function,
                             uint32_t line) {
    // Move strings into the lambda capture to avoid copies
    log_queue_.push([&log_sinks,
                     name = std::move(name),
                     level,
                     time_stamp_type,
                     message = std::move(message),
                     file = std::move(file),
                     function = std::move(function),
                     line] {
        for (auto& sink : log_sinks) {
            sink->log(name, level, time_stamp_type, message, file, function, line);
        }
    });
}

void LogDispatcher::flush(const std::vector<std::unique_ptr<ILogSink>>& log_sinks) {
    log_queue_worker_.flush();
    for (auto& sink : log_sinks) {
        sink->flush();
    }
}

}  // namespace log
}  // namespace vne
