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

#include <gtest/gtest.h>

#include "vertexnova/logging/core/log_stream.h"
#include "vertexnova/logging/core/logger_controller.h"
#include "vertexnova/logging/core/sync_logger.h"
#include "vertexnova/logging/core/console_log_sink.h"

namespace {
constexpr const char* kLoggerName = "TestLogger";
constexpr const char* kCategoryName = "TestCategory";
constexpr const char* kFileName = "TestFile";
constexpr const char* kFunctionName = "TestFunction";
constexpr uint32_t kLineNumber = 42;
}  // namespace

using namespace vne;

class LogStreamTest : public ::testing::Test {
   protected:
    void SetUp() override { log::LoggerController::unregisterAllLoggers(); }

    void TearDown() override { log::LoggerController::unregisterAllLoggers(); }
};

TEST_F(LogStreamTest, LogStreamConstructDestructWithoutLogger) {
    {
        log::LogStream
            log_stream(kLoggerName, kCategoryName, log::LogLevel::eInfo, log::TimeStampType::eLocal, kFileName, kFunctionName, kLineNumber);
        log_stream << "Test message";
    }
    SUCCEED();
}

TEST_F(LogStreamTest, LogStreamConstructDestructWithLogger) {
    auto logger = std::make_shared<log::SyncLogger>(kLoggerName);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));
    log::LoggerController::registerLogger(logger);
    {
        log::LogStream
            log_stream(kLoggerName, kCategoryName, log::LogLevel::eInfo, log::TimeStampType::eLocal, kFileName, kFunctionName, kLineNumber);
        log_stream << "Test message with logger";
    }

    auto retrieved_logger = log::LoggerController::getLogger(kLoggerName);
    ASSERT_NE(retrieved_logger, nullptr);
    ASSERT_EQ(logger, retrieved_logger);
}

TEST_F(LogStreamTest, LogStreamWithDifferentLogLevels) {
    auto logger = std::make_shared<log::SyncLogger>(kLoggerName);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));
    log::LoggerController::registerLogger(logger);

    logger->setCurrentLogLevel(log::LogLevel::eWarn);

    {
        log::LogStream
            log_stream(kLoggerName, kCategoryName, log::LogLevel::eInfo, log::TimeStampType::eLocal, kFileName, kFunctionName, kLineNumber);
        log_stream << "This message should not be logged";
    }

    {
        log::LogStream
            log_stream(kLoggerName, kCategoryName, log::LogLevel::eError, log::TimeStampType::eLocal, kFileName, kFunctionName, kLineNumber);
        log_stream << "This message should be logged";
    }

    auto retrieved_logger = log::LoggerController::getLogger(kLoggerName);
    ASSERT_NE(retrieved_logger, nullptr);
    ASSERT_EQ(logger, retrieved_logger);
}

TEST_F(LogStreamTest, LogStreamWithEmptyMessage) {
    auto logger = std::make_shared<log::SyncLogger>(kLoggerName);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));
    log::LoggerController::registerLogger(logger);

    {
        log::LogStream
            log_stream(kLoggerName, kCategoryName, log::LogLevel::eInfo, log::TimeStampType::eLocal, kFileName, kFunctionName, kLineNumber);
        // Empty message
    }

    auto retrieved_logger = log::LoggerController::getLogger(kLoggerName);
    ASSERT_NE(retrieved_logger, nullptr);
    ASSERT_EQ(logger, retrieved_logger);
}
