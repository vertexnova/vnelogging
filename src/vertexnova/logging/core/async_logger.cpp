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

#include "async_logger.h"

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

AsyncLogger::AsyncLogger(std::string logger_name)
    : logger_name_(std::move(logger_name))
    , current_log_level_(LogLevel::eInfo)
    , flush_level_(LogLevel::eError)
    , dispatcher_(std::make_unique<LogDispatcher>()) {}

AsyncLogger::~AsyncLogger() {}

void AsyncLogger::addLogSink(std::unique_ptr<ILogSink> log_sink) {
    log_sinks_.push_back(std::move(log_sink));
}

const std::vector<std::unique_ptr<ILogSink>>& AsyncLogger::getLogSinks() const {
    return log_sinks_;
}

void AsyncLogger::setCurrentLogLevel(LogLevel level) {
    current_log_level_ = level;
}

LogLevel AsyncLogger::getCurrentLogLevel() const {
    return current_log_level_;
}

void AsyncLogger::setFlushLevel(LogLevel level) {
    flush_level_ = level;
}

LogLevel AsyncLogger::getFlushLevel() const {
    return flush_level_;
}

void AsyncLogger::log(const std::string& category_name,
                      LogLevel level,
                      TimeStampType time_stamp_type,
                      const std::string& message,
                      const std::string& file,
                      const std::string& function,
                      uint32_t line) {
    if (level >= current_log_level_) {
        dispatcher_->dispatch(log_sinks_, category_name, level, time_stamp_type, message, file, function, line);
        if (level >= flush_level_) {
            dispatcher_->flush(log_sinks_);
        }
    }
}

void AsyncLogger::flush() {
    dispatcher_->flush(log_sinks_);
}

std::string AsyncLogger::getName() const {
    return logger_name_;
}

std::unique_ptr<ILogger> AsyncLogger::clone(const std::string& logger_name) const {
    auto cloned = std::make_unique<AsyncLogger>(logger_name);
    cloned->current_log_level_ = current_log_level_;
    cloned->flush_level_ = flush_level_;
    for (const auto& sink : log_sinks_) {
        cloned->log_sinks_.push_back(sink->clone());
    }
    return cloned;
}

}  // namespace log
}  // namespace vne
