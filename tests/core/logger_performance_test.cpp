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
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>

#include "vertexnova/logging/core/sync_logger.h"
#include "vertexnova/logging/core/async_logger.h"
#include "vertexnova/logging/core/console_log_sink.h"

#ifdef VNE_DEVELOPER_BUILD
using namespace vne;

// Redirects cout to a string stream for capturing console output
class CoutRedirect {
   public:
    CoutRedirect(std::streambuf* new_buffer)
        : old_(std::cout.rdbuf(new_buffer)) {}

    ~CoutRedirect() { std::cout.rdbuf(old_); }

   private:
    std::streambuf* old_;
};

class LoggerPerformanceTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Redirect std::cout to capture console output
        redirect_ = new CoutRedirect(cout_buffer_.rdbuf());
    }

    void TearDown() override { delete redirect_; }

   protected:
    // Helper function to log messages for performance tests
    template<typename LoggerType>
    void logMessages(LoggerType logger) {
        std::string message = "Logging test message.";

        auto start_time = std::chrono::steady_clock::now();

        for (uint32_t i = 0; i < kNumMessages; ++i) {
            logger->log("LogMessageName",
                        log::LogLevel::eInfo,
                        log::TimeStampType::eLocal,
                        message,
                        "TestFile",
                        "TestFunction",
                        i);
        }

        auto end_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        // Output the throughput metrics
        std::cout << "Logger Performance - Total Messages: " << kNumMessages << ", Elapsed Time (ms): " << elapsed_time
                  << ", Throughput (messages/sec): " << (kNumMessages * 1000 / elapsed_time) << std::endl;

        // Optionally, assert based on performance thresholds
        EXPECT_TRUE(elapsed_time < 1000);
    }

    // Helper function to log messages from multiple threads for thread safety tests
    template<typename LoggerType>
    void logMessagesConcurrently(LoggerType logger) {
        std::atomic<int> counter(0);

        // Function for concurrent logging
        auto logging_func = [&logger, &counter]() {
            for (uint32_t i = 0; i < kNumMessages; ++i) {
                logger->log("LogMessagesConcurrently",
                            log::LogLevel::eInfo,
                            log::TimeStampType::eLocal,
                            "Thread safe log message.",
                            "ThreadSafeLoggingFile",
                            "ThreadSafeLoggingFunction",
                            i);
                counter.fetch_add(1, std::memory_order_relaxed);  // Atomic increment for thread safety check
            }
        };

        std::vector<std::thread> threads;

        // Start threads
        for (int i = 0; i < kNumThreads; ++i) {
            threads.emplace_back(logging_func);
        }

        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }

        // Validate thread safety
        EXPECT_EQ(counter.load(std::memory_order_relaxed), kNumThreads * kNumMessages);
    }

   protected:
    static constexpr int kNumMessages = 10000;  // Number of messages to log in performance tests
    static constexpr int kNumThreads = 10;      // Number of threads for thread safety tests

   protected:
    std::stringstream cout_buffer_;
    CoutRedirect* redirect_;
};

// Performance tests for SyncLogger
TEST_F(LoggerPerformanceTest, SyncLoggerPerformance) {
    std::string logger_name = "SyncLogger";
    auto logger = std::make_shared<log::SyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));

    logMessages(logger);
}

// Performance tests for AsyncLogger
TEST_F(LoggerPerformanceTest, AsyncLoggerPerformance) {
    std::string logger_name = "AsyncLogger";
    auto logger = std::make_shared<log::AsyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));

    logMessages(logger);
}

// Thread safety tests for SyncLogger
TEST_F(LoggerPerformanceTest, SyncLoggerThreadSafety) {
    std::string logger_name = "SyncLogger";
    auto logger = std::make_shared<log::SyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));

    logMessagesConcurrently(logger);
}

// Thread safety tests for AsyncLogger
TEST_F(LoggerPerformanceTest, AsyncLoggerThreadSafety) {
    std::string logger_name = "AsyncLogger";
    auto logger = std::make_shared<log::AsyncLogger>(logger_name);
    auto console_sink = std::make_unique<log::ConsoleLogSink>();
    logger->addLogSink(std::move(console_sink));

    logMessagesConcurrently(logger);
}

#endif  // VNE_DEVELOPER_BUILD
