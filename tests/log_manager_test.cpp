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
#include "vertexnova/logging/log_manager.h"
#include "vertexnova/logging/core/sync_logger.h"
#include "vertexnova/logging/core/logger_controller.h"

#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>

using namespace vne;
namespace fs = std::filesystem;

namespace {
constexpr const char* kTestDir = "test_dir";
}  // namespace

class LogManagerTest : public ::testing::Test {
   protected:
    void SetUp() override { fs::create_directory(kTestDir); }

    void TearDown() override {
        try {
            int max_attempts = 3;
            for (int attempt = 0; attempt < max_attempts; ++attempt) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    fs::remove_all(kTestDir);
                    break;
                } catch (const fs::filesystem_error& e) {
                    std::cerr << "Attempt " << attempt + 1 << " failed: " << e.what() << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception during TearDown(): " << e.what() << std::endl;
        }
    }
};

TEST_F(LogManagerTest, DefaultLoggerInitialization) {
    {
        std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
        auto logger = log_manager->getLogger("VNE_DEFAULT_SYNC_LOGGER");
        ASSERT_EQ(logger, nullptr);
    }
    {
        std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
        log_manager->createLogger("VNE_DEFAULT_ASYNC_LOGGER", true);  // Explicitly set as async
        auto logger = log_manager->getLogger("VNE_DEFAULT_ASYNC_LOGGER");
        ASSERT_NE(logger, nullptr);
        EXPECT_EQ(logger->getName(), "VNE_DEFAULT_ASYNC_LOGGER");
    }
}

TEST_F(LogManagerTest, CreateLogger) {
    {
        std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
        auto logger = log_manager->createLogger("TestLogger", false);  // Explicitly set as sync
        ASSERT_NE(logger, nullptr);
        EXPECT_EQ(logger->getName(), "TestLogger");
    }
    {
        std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
        auto logger = log_manager->createLogger("TestAsyncLogger", true);  // Explicitly set as async
        ASSERT_NE(logger, nullptr);
        EXPECT_EQ(logger->getName(), "TestAsyncLogger");
    }
}

TEST_F(LogManagerTest, AddConsoleSink) {
    std::string logger_name = "ConsoleSinkLogger";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->addConsoleSink(logger_name);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);
    auto& sinks = logger->getLogSinks();
    bool found_console_sink = false;
    for (const auto& sink : sinks) {
        if (dynamic_cast<log::ConsoleLogSink*>(sink.get()) != nullptr) {
            found_console_sink = true;
            break;
        }
    }
    EXPECT_TRUE(found_console_sink);
}

TEST_F(LogManagerTest, AddFileSink) {
    std::string logger_name = "FileSinkLogger";
    std::string test_file = std::string(kTestDir) + "/" + "test_file.txt";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->addFileSink(logger_name, test_file);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);
    auto& sinks = logger->getLogSinks();
    bool found_file_sink = false;
    for (const auto& sink : sinks) {
        if (dynamic_cast<log::FileLogSink*>(sink.get()) != nullptr) {
            found_file_sink = true;
            break;
        }
    }
    EXPECT_TRUE(found_file_sink);
}

TEST_F(LogManagerTest, SetConsolePattern) {
    std::string logger_name = "PatternLogger";
    std::string pattern = "%v [%x] [%l] %!";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->addConsoleSink(logger_name);
    log_manager->setConsolePattern(logger_name, pattern);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);
    auto& sinks = logger->getLogSinks();
    for (const auto& sink : sinks) {
        auto console_sink = dynamic_cast<log::ConsoleLogSink*>(sink.get());
        if (console_sink) {
            EXPECT_EQ(console_sink->getPattern(), pattern);
        }
    }
}

TEST_F(LogManagerTest, SetFilePattern) {
    std::string logger_name = "PatternLogger";
    std::string pattern = "%v [%x] [%l] %!";
    std::string test_file = std::string(kTestDir) + "/" + "test_file.txt";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->addFileSink(logger_name, test_file);
    log_manager->setFilePattern(logger_name, pattern);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);
    auto& sinks = logger->getLogSinks();
    for (const auto& sink : sinks) {
        auto file_sink = dynamic_cast<log::FileLogSink*>(sink.get());
        if (file_sink) {
            EXPECT_EQ(file_sink->getPattern(), pattern);
        }
    }
}

TEST_F(LogManagerTest, SetLogLevel) {
    std::string logger_name = "LevelLogger";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->setLogLevel(logger_name, log::LogLevel::eDebug);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);
    EXPECT_EQ(logger->getCurrentLogLevel(), log::LogLevel::eDebug);
}

TEST_F(LogManagerTest, Finalize) {
    std::string logger_name = "FinalizeLogger";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    auto logger = log_manager->getLogger(logger_name);
    EXPECT_NE(logger, nullptr);
    log_manager->finalize();
    logger = log_manager->getLogger(logger_name);
    EXPECT_EQ(logger, nullptr);
}

TEST_F(LogManagerTest, IsLoggerAsync) {
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();

    // Create sync logger
    std::string sync_logger_name = "SyncLogger";
    log_manager->createLogger(sync_logger_name, false);
    EXPECT_FALSE(log_manager->isLoggerAsync(sync_logger_name));

    // Create async logger
    std::string async_logger_name = "AsyncLogger";
    log_manager->createLogger(async_logger_name, true);
    EXPECT_TRUE(log_manager->isLoggerAsync(async_logger_name));

    // Check non-existent logger
    EXPECT_FALSE(log_manager->isLoggerAsync("NonExistentLogger"));
}

TEST_F(LogManagerTest, SetFlushLevel) {
    std::string logger_name = "FlushLevelLogger";
    std::string test_file = std::string(kTestDir) + "/" + "flush_level_test.txt";
    std::unique_ptr<log::LogManager> log_manager = std::make_unique<log::LogManager>();
    log_manager->createLogger(logger_name, false);
    log_manager->addFileSink(logger_name, test_file);

    // Set flush level to eWarn
    log_manager->setFlushLevel(logger_name, log::LogLevel::eWarn);
    auto logger = log_manager->getLogger(logger_name);
    ASSERT_NE(logger, nullptr);

    EXPECT_EQ(logger->getFlushLevel(), log::LogLevel::eWarn);
}
