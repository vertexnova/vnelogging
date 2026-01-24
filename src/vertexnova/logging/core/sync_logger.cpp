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

#include "sync_logger.h"

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

SyncLogger::SyncLogger(const std::string& logger_name)
    : logger_name_(logger_name)
    , current_log_level_(LogLevel::eInfo)
    , flush_level_(LogLevel::eError) {}

SyncLogger::~SyncLogger() {}

void SyncLogger::addLogSink(std::unique_ptr<ILogSink> log_sink) {
    log_sinks_.push_back(std::move(log_sink));
}

const std::vector<std::unique_ptr<ILogSink>>& SyncLogger::getLogSinks() const {
    return log_sinks_;
}

void SyncLogger::setCurrentLogLevel(LogLevel level) {
    current_log_level_ = level;
}

LogLevel SyncLogger::getCurrentLogLevel() const {
    return current_log_level_;
}

void SyncLogger::setFlushLevel(LogLevel level) {
    flush_level_ = level;
}

LogLevel SyncLogger::getFlushLevel() const {
    return flush_level_;
}

void SyncLogger::log(const std::string& category_name,
                     LogLevel level,
                     TimeStampType time_stamp_type,
                     const std::string& message,
                     const std::string& file,
                     const std::string& function,
                     uint32_t line) {
    if (level >= current_log_level_) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& sink : log_sinks_) {
            sink->log(category_name, level, time_stamp_type, message, file, function, line);
        }
        if (level >= flush_level_) {
            for (auto& sink : log_sinks_) {
                sink->flush();
            }
        }
    }
}

void SyncLogger::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& sink : log_sinks_) {
        sink->flush();
    }
}

std::string SyncLogger::getName() const {
    return logger_name_;
}

std::unique_ptr<ILogger> SyncLogger::clone(const std::string& logger_name) const {
    return std::unique_ptr<SyncLogger>(new SyncLogger(logger_name));
}

}  // namespace log
}  // namespace vne
