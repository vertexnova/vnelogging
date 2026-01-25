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

#include "log_stream.h"
#include "logger_controller.h"

#include <utility>

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

LogStream::LogStream(const char* logger_name,
                     std::string category,
                     LogLevel level,
                     TimeStampType time_stamp_type,
                     std::string file,
                     std::string function,
                     uint32_t line)
    : logger_name_(logger_name)
    , category_(std::move(category))
    , log_level_(level)
    , time_stamp_type_(time_stamp_type)
    , file_(std::move(file))
    , function_(std::move(function))
    , line_(line) {}

LogStream::~LogStream() {
    std::shared_ptr<ILogger> logger = LoggerController::getLogger(logger_name_);
    if (logger) {
        if (log_level_ >= logger->getCurrentLogLevel()) {
            logger->log(category_, log_level_, time_stamp_type_, msg_stream_.str(), file_, function_, line_);
        }
    }
}

}  // namespace log
}  // namespace vne
