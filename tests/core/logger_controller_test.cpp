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

#include "vertexnova/logging/core/logger_controller.h"
#include "vertexnova/logging/core/sync_logger.h"
#include "vertexnova/logging/core/async_logger.h"
#include "vertexnova/logging/core/console_log_sink.h"
#include "vertexnova/logging/core/file_log_sink.h"

#include <memory>
#include <vector>
#include <string>
#include <algorithm>

using namespace vne;

namespace {

bool isStringInVector(const std::vector<std::string>& vec, const std::string& str) {
    auto it = std::find(vec.begin(), vec.end(), str);
    return it != vec.end();
}
}  // namespace

class LoggerControllerTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Ensure no logger is registered before each test
        log::LoggerController::unregisterAllLoggers();
    }

    void TearDown() override {
        // Ensure no logger is registered after each test
        log::LoggerController::unregisterAllLoggers();
    }
};

// Synchronous Logger Tests
TEST_F(LoggerControllerTest, RegisterSyncLogger) {
    std::string logger_name = "SyncTestLogger";
    auto logger = std::make_shared<log::SyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_NE(retrieved_logger, nullptr);
    EXPECT_EQ(logger, retrieved_logger);
}

TEST_F(LoggerControllerTest, GetSyncLoggerWithoutRegister) {
    auto retrieved_logger = log::LoggerController::getLogger("UnknownLogger");
    EXPECT_EQ(retrieved_logger, nullptr);
}

TEST_F(LoggerControllerTest, UnregisterSyncLogger) {
    std::string logger_name = "SyncTestLogger";
    auto logger = std::make_shared<log::SyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    log::LoggerController::unregisterLogger(logger_name);
    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(retrieved_logger, nullptr);
}

TEST_F(LoggerControllerTest, RegisterMultipleSyncLoggers) {
    auto logger1 = std::make_shared<log::SyncLogger>("logger1");
    auto logger2 = std::make_shared<log::SyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    auto retrieved_logger1 = log::LoggerController::getLogger("logger1");
    EXPECT_EQ(logger1, retrieved_logger1);

    log::LoggerController::registerLogger(logger2);
    auto retrieved_logger2 = log::LoggerController::getLogger("logger2");
    EXPECT_EQ(logger2, retrieved_logger2);
}

TEST_F(LoggerControllerTest, UnregisterAllSyncLoggers) {
    auto logger1 = std::make_shared<log::SyncLogger>("logger1");
    auto logger2 = std::make_shared<log::SyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    log::LoggerController::registerLogger(logger2);
    EXPECT_NE(log::LoggerController::getLogger("logger1"), nullptr);
    EXPECT_NE(log::LoggerController::getLogger("logger2"), nullptr);

    log::LoggerController::unregisterAllLoggers();
    EXPECT_EQ(log::LoggerController::getLogger("logger1"), nullptr);
    EXPECT_EQ(log::LoggerController::getLogger("logger2"), nullptr);
}

TEST_F(LoggerControllerTest, SetAndGetCurrentSyncLogger) {
    auto logger1 = std::make_shared<log::SyncLogger>("logger1");
    auto logger2 = std::make_shared<log::SyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    auto retrieved_logger1 = log::LoggerController::getLogger("logger1");
    EXPECT_EQ(logger1, retrieved_logger1);

    log::LoggerController::registerLogger(logger2);
    auto retrieved_logger2 = log::LoggerController::getLogger("logger2");
    EXPECT_EQ(logger2, retrieved_logger2);
}

TEST_F(LoggerControllerTest, GetSyncLoggerNames) {
    auto logger1 = std::make_shared<log::SyncLogger>("logger1");
    auto logger2 = std::make_shared<log::SyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    log::LoggerController::registerLogger(logger2);

    std::vector<std::string> logger_names = log::LoggerController::getLoggerNames();
    EXPECT_EQ(logger_names.size(), 2);

    EXPECT_TRUE(isStringInVector(logger_names, "logger1"));
    EXPECT_TRUE(isStringInVector(logger_names, "logger2"));
    EXPECT_FALSE(isStringInVector(logger_names, "unknown"));
}

TEST_F(LoggerControllerTest, SyncLoggerPersistenceAcrossMethods) {
    std::string logger_name = "TestLogger";
    auto logger = std::make_shared<log::SyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(logger, retrieved_logger);

    log::LoggerController::unregisterLogger(logger_name);
    retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(retrieved_logger, nullptr);
}

// Asynchronous Logger Tests
TEST_F(LoggerControllerTest, RegisterAsyncLogger) {
    std::string logger_name = "AsyncTestLogger";
    auto logger = std::make_shared<log::AsyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_NE(retrieved_logger, nullptr);
    EXPECT_EQ(logger, retrieved_logger);
}

TEST_F(LoggerControllerTest, GetAsyncLoggerWithoutRegister) {
    auto retrieved_logger = log::LoggerController::getLogger("UnknownLogger");
    EXPECT_EQ(retrieved_logger, nullptr);
}

TEST_F(LoggerControllerTest, UnregisterAsyncLogger) {
    std::string logger_name = "AsyncTestLogger";
    auto logger = std::make_shared<log::AsyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    log::LoggerController::unregisterLogger(logger_name);
    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(retrieved_logger, nullptr);
}

TEST_F(LoggerControllerTest, RegisterMultipleAsyncLoggers) {
    auto logger1 = std::make_shared<log::AsyncLogger>("logger1");
    auto logger2 = std::make_shared<log::AsyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    auto retrieved_logger1 = log::LoggerController::getLogger("logger1");
    EXPECT_EQ(logger1, retrieved_logger1);

    log::LoggerController::registerLogger(logger2);
    auto retrieved_logger2 = log::LoggerController::getLogger("logger2");
    EXPECT_EQ(logger2, retrieved_logger2);
}

TEST_F(LoggerControllerTest, UnregisterAllAsyncLoggers) {
    auto logger1 = std::make_shared<log::AsyncLogger>("logger1");
    auto logger2 = std::make_shared<log::AsyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    log::LoggerController::registerLogger(logger2);
    EXPECT_NE(log::LoggerController::getLogger("logger1"), nullptr);
    EXPECT_NE(log::LoggerController::getLogger("logger2"), nullptr);

    log::LoggerController::unregisterAllLoggers();
    EXPECT_EQ(log::LoggerController::getLogger("logger1"), nullptr);
    EXPECT_EQ(log::LoggerController::getLogger("logger2"), nullptr);
}

TEST_F(LoggerControllerTest, SetAndGetCurrentAsyncLogger) {
    auto logger1 = std::make_shared<log::AsyncLogger>("logger1");
    auto logger2 = std::make_shared<log::AsyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    auto retrieved_logger1 = log::LoggerController::getLogger("logger1");
    EXPECT_EQ(logger1, retrieved_logger1);

    log::LoggerController::registerLogger(logger2);
    auto retrieved_logger2 = log::LoggerController::getLogger("logger2");
    EXPECT_EQ(logger2, retrieved_logger2);
}

TEST_F(LoggerControllerTest, GetAsyncLoggerNames) {
    auto logger1 = std::make_shared<log::AsyncLogger>("logger1");
    auto logger2 = std::make_shared<log::AsyncLogger>("logger2");

    log::LoggerController::registerLogger(logger1);
    log::LoggerController::registerLogger(logger2);

    std::vector<std::string> logger_names = log::LoggerController::getLoggerNames();
    EXPECT_EQ(logger_names.size(), 2);

    EXPECT_TRUE(isStringInVector(logger_names, "logger1"));
    EXPECT_TRUE(isStringInVector(logger_names, "logger2"));
    EXPECT_FALSE(isStringInVector(logger_names, "unknown"));
}

TEST_F(LoggerControllerTest, AsyncLoggerPersistenceAcrossMethods) {
    std::string logger_name = "TestLogger";
    auto logger = std::make_shared<log::AsyncLogger>(logger_name);
    log::LoggerController::registerLogger(logger);

    auto retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(logger, retrieved_logger);

    log::LoggerController::unregisterLogger(logger_name);
    retrieved_logger = log::LoggerController::getLogger(logger_name);
    EXPECT_EQ(retrieved_logger, nullptr);
}
