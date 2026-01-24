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

#include "vertexnova/logging/core/async_logger.h"
#include "vertexnova/logging/core/console_log_sink.h"
#include "vertexnova/logging/core/file_log_sink.h"

#include <fstream>
#include <memory>
#include <filesystem>
#include <sstream>
#include <thread>
#include <chrono>

using namespace vne;
namespace fs = std::filesystem;

namespace {
constexpr const char* kTestDir = "test_dir";

constexpr const char* kLoggerCatName = "TestLogger";
constexpr const char* kFileName = "TestFile";
constexpr const char* kFunctionName = "TestFunction";
constexpr uint32_t kLineNumber = 42;
}  // namespace

// Helper function to capture std::cout output
class CoutRedirect {
   public:
    CoutRedirect(std::streambuf* new_buffer)
        : old_(std::cout.rdbuf(new_buffer)) {}
    ~CoutRedirect() { std::cout.rdbuf(old_); }

   private:
    std::streambuf* old_;
};

class AsyncLoggerTest : public ::testing::Test {
   protected:
    void SetUp() override { }

    void TearDown() override {
        try {
            int max_attempts = 3;
            for (int attempt = 0; attempt < max_attempts; ++attempt) {
                try {
                    std::filesystem::remove_all(kTestDir);
                    break;
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Attempt " << attempt + 1 << " failed: " << e.what() << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception during TearDown(): " << e.what() << std::endl;
        }
    }

   protected:
};

TEST_F(AsyncLoggerTest, AddLogSink) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));
    EXPECT_EQ(logger->getCurrentLogLevel(), log::LogLevel::eInfo);
}

TEST_F(AsyncLoggerTest, GetLogSinks) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    std::string test_file = std::string(kTestDir) + "/" + "test_file.txt";
    auto file_sink = std::make_unique<log::FileLogSink>(test_file, false);

    logger->addLogSink(std::move(console_sink));
    logger->addLogSink(std::move(file_sink));

    auto& log_sinks = logger->getLogSinks();
    EXPECT_EQ(log_sinks.size(), 2);
}

TEST_F(AsyncLoggerTest, SetGetCurrentLogLevel) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    logger->setCurrentLogLevel(log::LogLevel::eDebug);
    EXPECT_EQ(logger->getCurrentLogLevel(), log::LogLevel::eDebug);
}

TEST_F(AsyncLoggerTest, GetLoggerName) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    EXPECT_EQ(logger_name, logger->getName());
}

TEST_F(AsyncLoggerTest, GetLogger) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    EXPECT_NE(nullptr, logger->clone(logger_name));
}

TEST_F(AsyncLoggerTest, LogWithNoSinks) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    // Should not throw or do anything when no sinks are added
    EXPECT_NO_THROW(
        logger->log(kLoggerCatName, log::LogLevel::eInfo, log::TimeStampType::eUtc, "Test message", kFileName, kFunctionName, kLineNumber));
}

TEST_F(AsyncLoggerTest, LogMessage) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);

    std::stringstream buffer;
    CoutRedirect redirect(buffer.rdbuf());

    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));

    logger->log(kLoggerCatName, log::LogLevel::eInfo, log::TimeStampType::eUtc, "Test message", kFileName, kFunctionName, kLineNumber);
    logger->flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // wait for few ms

    std::string output = buffer.str();

    EXPECT_TRUE(output.find("Test message") != std::string::npos);
}

TEST_F(AsyncLoggerTest, LogBelowCurrentLogLevel) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);

    std::stringstream buffer;
    CoutRedirect redirect(buffer.rdbuf());

    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));
    logger->setCurrentLogLevel(log::LogLevel::eError);

    logger->log(kLoggerCatName,
                log::LogLevel::eInfo,
                log::TimeStampType::eUtc,
                "This message should not be logged",
                kFileName,
                kFunctionName,
                kLineNumber);

    std::string output = buffer.str();
    EXPECT_TRUE(output.empty());
}

TEST_F(AsyncLoggerTest, Flush) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);

    std::string test_file = std::string(kTestDir) + "/" + "test_file_flush.txt";
    auto file_sink = std::make_unique<log::FileLogSink>(test_file, false);
    logger->addLogSink(std::move(file_sink));

    logger->log(kLoggerCatName, log::LogLevel::eInfo, log::TimeStampType::eUtc, "Test message", kFileName, kFunctionName, kLineNumber);
    logger->flush();
    logger.reset();                                               // Ensure async logger is destroyed and threads are joined
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Ensure file handles are released

    std::ifstream log_file(test_file);
    std::stringstream buffer;
    buffer << log_file.rdbuf();
    log_file.close();
    std::string file_content = buffer.str();

    EXPECT_TRUE(file_content.find("Test message") != std::string::npos);
}

TEST_F(AsyncLoggerTest, FlushLevel) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);
    std::string test_file = std::string(kTestDir) + "/" + "flush_level_test.txt";
    auto file_sink = std::make_unique<log::FileLogSink>(test_file, false);
    logger->addLogSink(std::move(file_sink));

    // Set flush level to eWarn
    logger->setFlushLevel(log::LogLevel::eWarn);

    // Log below flush level (should not flush immediately)
    logger->log(kLoggerCatName,
                log::LogLevel::eInfo,
                log::TimeStampType::eLocal,
                "Should not flush yet",
                kFileName,
                kFunctionName,
                kLineNumber);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger->flush();  // Ensure all messages are processed before checking
    logger.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream log_file1(test_file);
    std::stringstream buffer1;
    buffer1 << log_file1.rdbuf();
    log_file1.close();
    std::string file_content1 = buffer1.str();
    EXPECT_TRUE(file_content1.find("Should not flush yet") != std::string::npos);

    // Log at flush level (should flush immediately)
    logger = std::make_shared<log::AsyncLogger>(logger_name);
    auto file_sink2 = std::make_unique<log::FileLogSink>(test_file, true);  // append mode
    logger->addLogSink(std::move(file_sink2));
    logger->setFlushLevel(log::LogLevel::eWarn);

    logger->log(kLoggerCatName, log::LogLevel::eWarn, log::TimeStampType::eLocal, "Should flush now", kFileName, kFunctionName, kLineNumber);
    logger->flush();
    logger.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream log_file2(test_file);
    std::stringstream buffer2;
    buffer2 << log_file2.rdbuf();
    log_file2.close();
    std::string file_content2 = buffer2.str();
    EXPECT_TRUE(file_content2.find("Should flush now") != std::string::npos);
}

TEST_F(AsyncLoggerTest, AddMultipleLogSinks) {
    std::string logger_name = "AsyncTestLogger";
    std::shared_ptr<log::AsyncLogger> logger = std::make_shared<log::AsyncLogger>(logger_name);

    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    std::string test_file = std::string(kTestDir) + "/" + "test_file_multiple_sinks.txt";
    auto file_sink = std::make_unique<log::FileLogSink>(test_file, false);

    logger->addLogSink(std::move(console_sink));
    logger->addLogSink(std::move(file_sink));

    // Set flush level to eInfo so this message is flushed immediately
    logger->setFlushLevel(log::LogLevel::eInfo);

    logger->log(kLoggerCatName,
                log::LogLevel::eInfo,
                log::TimeStampType::eLocal,
                "Test message for multiple sinks",
                kFileName,
                kFunctionName,
                kLineNumber);
    logger->flush();
    logger.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream log_file(test_file);
    std::stringstream buffer;
    buffer << log_file.rdbuf();
    log_file.close();
    std::string file_content = buffer.str();

    EXPECT_TRUE(file_content.find("Test message for multiple sinks") != std::string::npos);
}
