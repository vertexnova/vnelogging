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

#include "console_log_sink.h"
#include "log_formatter.h"
#include "text_color.h"

#include <iostream>
#include <memory>
#include <sstream>

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

ConsoleLogSink::ConsoleLogSink()
    : pattern_("%x [%l] %v") {}

ConsoleLogSink::~ConsoleLogSink() {}

void ConsoleLogSink::log(const std::string& name,
                         LogLevel level,
                         TimeStampType time_stamp_type,
                         const std::string& message,
                         const std::string& file,
                         const std::string& function,
                         uint32_t line) {
    std::string formatted_log =
        LogFormatter::format(name, level, time_stamp_type, message, file, function, line, pattern_);

    // Define colors for each log level
    TextColor color;
    switch (level) {
        case LogLevel::eTrace:
            color = TextColor(DisplayAttributes::eNormal, FGColorCode::eLightGray, BGColorCode::eDefault);
            break;
        case LogLevel::eDebug:
            color = TextColor(DisplayAttributes::eNormal, FGColorCode::eBlue, BGColorCode::eDefault);
            break;
        case LogLevel::eInfo:
            color = TextColor(DisplayAttributes::eNormal, FGColorCode::eGreen, BGColorCode::eDefault);
            break;
        case LogLevel::eWarn:
            color = TextColor(DisplayAttributes::eBold, FGColorCode::eYellow, BGColorCode::eDefault);
            break;
        case LogLevel::eError:
            color = TextColor(DisplayAttributes::eBold, FGColorCode::eRed, BGColorCode::eDefault);
            break;
        case LogLevel::eFatal:
            color = TextColor(DisplayAttributes::eBold, FGColorCode::eMagenta, BGColorCode::eDefault);
            break;
        default:
            color = TextColor(DisplayAttributes::eNormal, FGColorCode::eDefault, BGColorCode::eDefault);
            break;
    }

    std::ostringstream oss;
    oss << color << formatted_log << getResetSequence() << '\n';

    // Single atomic write to stdout
    std::cout << oss.str();
}

void ConsoleLogSink::flush() {
    // No need to flush for console log
}

std::string ConsoleLogSink::getPattern() const {
    return pattern_;
}

void ConsoleLogSink::setPattern(const std::string& pattern) {
    pattern_ = pattern;
}

std::unique_ptr<ILogSink> ConsoleLogSink::clone() const {
    return std::make_unique<ConsoleLogSink>();
}

}  // namespace log
}  // namespace vne
