/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   JULY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "vertexnova/logging/core/log_formatter.h"
#include "vertexnova/logging/core/log_level.h"

#include <gtest/gtest.h>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

class LogFormatterTest : public ::testing::Test {
   protected:
    std::string getCurrentTime() const {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* ptm = std::localtime(&now_c);
        std::ostringstream oss;
        oss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

TEST_F(LogFormatterTest, FormatTimestamp) {
    std::string format = "%x";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, getCurrentTime());
}

TEST_F(LogFormatterTest, FormatLoggerName) {
    std::string format = "%n";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, logger_name);
}

TEST_F(LogFormatterTest, FormatLogLevel) {
    std::string format = "%l";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, "INFO");
}

TEST_F(LogFormatterTest, FormatFunction) {
    std::string format = "%!";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, function);
}

TEST_F(LogFormatterTest, FormatFile) {
    std::string format = "%$";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, file);
}

TEST_F(LogFormatterTest, FormatLine) {
    std::string format = "%#";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, std::to_string(line));
}

TEST_F(LogFormatterTest, FormatMessage) {
    std::string format = "%v";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, message);
}

TEST_F(LogFormatterTest, FormatThreadID) {
    std::string format = "%t";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_NE(formatted.find("Thread-"), std::string::npos);
}

TEST_F(LogFormatterTest, FormatComplex) {
    std::string format = "%x [%l] [%n] [%$] [%!] %v:%#";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    std::string expected = getCurrentTime() + " [INFO] [TestLogger] [TestFile] [TestFunction] Test message:42";
    EXPECT_EQ(formatted, expected);
}

TEST_F(LogFormatterTest, FormatUnknown) {
    std::string format = "%unknown";
    std::string logger_name = "TestLogger";
    std::string message = "Test message";
    std::string file = "TestFile";
    std::string function = "TestFunction";
    uint32_t line = 42;
    auto log_level = vne::log::LogLevel::eInfo;
    auto time_stamp_type = vne::log::TimeStampType::eLocal;

    std::string formatted =
        vne::log::LogFormatter::format(logger_name, log_level, time_stamp_type, message, file, function, line, format);
    EXPECT_EQ(formatted, format);
}
