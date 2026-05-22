/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Tests for logging path utility functions
 * ----------------------------------------------------------------------
 */

#include <gtest/gtest.h>

#include "vertexnova/logging/logging.h"

#include <filesystem>
#include <fstream>

namespace vne::log {

class LoggingPathTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Clean up any existing test directories
        cleanupTestDirectories();
    }

    void TearDown() override {
        // Shutdown logging and clean up test files
        Logging::shutdown();
        cleanupTestDirectories();
    }

    void cleanupTestDirectories() {
        std::error_code ec;
        std::filesystem::remove_all("test_logs", ec);
        std::filesystem::remove_all("logs", ec);
        std::filesystem::remove("test_log.log", ec);
    }
};

//==============================================================================
// getPlatformSpecificLogDirectory Tests
//==============================================================================

TEST_F(LoggingPathTest, GetPlatformSpecificLogDirectory_ReturnsNonEmptyPath) {
    std::string log_dir = Logging::getPlatformSpecificLogDirectory();

#ifdef VNE_PLATFORM_WEB
    // Web platform returns empty string (no file logging)
    EXPECT_TRUE(log_dir.empty());
#else
    EXPECT_FALSE(log_dir.empty());
#endif
}

TEST_F(LoggingPathTest, GetPlatformSpecificLogDirectory_ContainsVertexNova) {
    std::string log_dir = Logging::getPlatformSpecificLogDirectory();

#ifndef VNE_PLATFORM_WEB
    // Should contain VertexNova or logs in the path
    bool contains_expected =
        log_dir.find("VertexNova") != std::string::npos || log_dir.find("logs") != std::string::npos;
    EXPECT_TRUE(contains_expected) << "Log directory: " << log_dir;
#endif
}

TEST_F(LoggingPathTest, GetLogDirectory_DelegatesToPlatformSpecific) {
    std::string log_dir1 = Logging::getLogDirectory();
    std::string log_dir2 = Logging::getPlatformSpecificLogDirectory();
    EXPECT_EQ(log_dir1, log_dir2);
}

//==============================================================================
// ensureLogDirectoryExists Tests
//==============================================================================

TEST_F(LoggingPathTest, EnsureLogDirectoryExists_CreatesDirectory) {
    const std::string test_dir = "test_logs/subdir/deep";

    EXPECT_FALSE(std::filesystem::exists(test_dir));

    bool result = Logging::ensureLogDirectoryExists(test_dir);

    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(test_dir));
    EXPECT_TRUE(std::filesystem::is_directory(test_dir));
}

TEST_F(LoggingPathTest, EnsureLogDirectoryExists_ReturnsTrueIfExists) {
    const std::string test_dir = "test_logs";

    std::filesystem::create_directories(test_dir);
    EXPECT_TRUE(std::filesystem::exists(test_dir));

    bool result = Logging::ensureLogDirectoryExists(test_dir);

    EXPECT_TRUE(result);
}

TEST_F(LoggingPathTest, EnsureLogDirectoryExists_ReturnsFalseForEmptyPath) {
    bool result = Logging::ensureLogDirectoryExists("");
    EXPECT_FALSE(result);
}

//==============================================================================
// createLoggingFolder Tests
//==============================================================================

TEST_F(LoggingPathTest, CreateLoggingFolder_CreatesTimestampedDirectory) {
    const std::string base_dir = "test_logs";
    const std::string filename = "test.log";

    std::string log_path = Logging::createLoggingFolder(base_dir, filename);

    EXPECT_FALSE(log_path.empty());
    EXPECT_TRUE(log_path.find(filename) != std::string::npos);

    // Extract directory from path
    std::filesystem::path path(log_path);
    std::filesystem::path parent = path.parent_path();

    EXPECT_TRUE(std::filesystem::exists(parent));
}

TEST_F(LoggingPathTest, CreateLoggingFolder_TimestampedDirContainsDate) {
    const std::string base_dir = "test_logs";
    const std::string filename = "test.log";

    std::string log_path = Logging::createLoggingFolder(base_dir, filename);

    // Should contain a date pattern like "2026-01-24"
    EXPECT_TRUE(log_path.find("202") != std::string::npos) << "Path should contain year: " << log_path;
}

//==============================================================================
// Integration Tests - File Logging with Cleanup
//==============================================================================

TEST_F(LoggingPathTest, FileLogging_CreatesAndWritesToFile) {
    const std::string log_file = "test_logs/integration_test.log";

    // Ensure directory exists
    Logging::ensureLogDirectoryExists("test_logs");

    // Configure logger with file sink
    LoggerConfig config;
    config.name = "test_logger";
    config.sink = LogSinkType::eFile;
    config.file_path = log_file;
    config.log_level = LogLevel::eTrace;
    config.async = false;

    Logging::configureLogger(config);

    // Log a message
    auto logger = Logging::getLogger("test_logger");
    ASSERT_NE(logger, nullptr);

    logger->log("test", LogLevel::eInfo, TimeStampType::eLocal, "Test message", __FILE__, __FUNCTION__, __LINE__);
    logger->flush();

    // Verify file was created
    EXPECT_TRUE(std::filesystem::exists(log_file)) << "Log file should exist: " << log_file;

    // Verify file has content
    if (std::filesystem::exists(log_file)) {
        auto file_size = std::filesystem::file_size(log_file);
        EXPECT_GT(file_size, 0) << "Log file should have content";
    }
}

TEST_F(LoggingPathTest, FileLogging_MultipleMessages) {
    const std::string log_file = "test_logs/multi_message.log";

    Logging::ensureLogDirectoryExists("test_logs");

    LoggerConfig config;
    config.name = "multi_logger";
    config.sink = LogSinkType::eFile;
    config.file_path = log_file;
    config.log_level = LogLevel::eTrace;
    config.async = false;

    Logging::configureLogger(config);

    auto logger = Logging::getLogger("multi_logger");
    ASSERT_NE(logger, nullptr);

    // Log multiple messages
    for (int i = 0; i < 10; ++i) {
        logger->log("test",
                    LogLevel::eInfo,
                    TimeStampType::eLocal,
                    "Message " + std::to_string(i),
                    __FILE__,
                    __FUNCTION__,
                    __LINE__);
    }
    logger->flush();

    // Verify file exists and has content
    EXPECT_TRUE(std::filesystem::exists(log_file));

    // Read file and count lines
    std::ifstream file(log_file);
    int line_count = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++line_count;
    }

    EXPECT_EQ(line_count, 10) << "Should have 10 log lines";
}

//==============================================================================
// defaultLoggerConfig Tests
//==============================================================================

TEST_F(LoggingPathTest, DefaultLoggerConfig_HasValidDefaults) {
    LoggerConfig config = Logging::defaultLoggerConfig();

    EXPECT_EQ(config.name, kDefaultLoggerName);
    EXPECT_FALSE(config.console_pattern.empty());
    EXPECT_FALSE(config.file_pattern.empty());
    EXPECT_EQ(config.log_level, LogLevel::eInfo);
    EXPECT_EQ(config.flush_level, LogLevel::eError);
    EXPECT_FALSE(config.async);

#ifdef VNE_PLATFORM_WEB
    EXPECT_EQ(config.sink, LogSinkType::eConsole);
    EXPECT_TRUE(config.file_path.empty());
#else
    EXPECT_EQ(config.sink, LogSinkType::eConsole);
#endif
}

}  // namespace vne::log
