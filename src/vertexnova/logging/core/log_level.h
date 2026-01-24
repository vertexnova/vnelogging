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

#include <iostream>

namespace vne::log {

/**
 * @enum LogLevel
 *
 * @brief Log severity levels
 */
enum class LogLevel {
    eTrace = 0,  //!< Trace level message (0)
    eDebug = 1,  //!< Debug-level message (1)
    eInfo = 2,   //!< Info-level message (2)
    eWarn = 3,   //!< Warning-level message (3)
    eError = 4,  //!< Error-level message (4)
    eFatal = 5   //!< Fatal-level message (5)
};

/**
 * @brief Stream operator for LogLevel enumeration.
 *
 * @param stream Output stream.
 * @param level Log level enumeration.
 * @return Output stream.
 */
inline std::ostream& operator<<(std::ostream& stream, const LogLevel& level) {
    switch (level) {
        case LogLevel::eTrace:
            stream << "TRACE";
            break;
        case LogLevel::eDebug:
            stream << "DEBUG";
            break;
        case LogLevel::eInfo:
            stream << "INFO";
            break;
        case LogLevel::eWarn:
            stream << "WARN";
            break;
        case LogLevel::eError:
            stream << "ERROR";
            break;
        case LogLevel::eFatal:
            stream << "FATAL";
            break;
        default:
            stream << "UNKNOWN";
            break;
    }
    return stream;
}

}  // namespace vne::log
