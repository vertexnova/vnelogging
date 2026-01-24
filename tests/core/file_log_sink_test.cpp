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
#include <filesystem>
#include <fstream>
#include <thread>

#include "vertexnova/logging/core/file_log_sink.h"

using namespace vne;
namespace fs = std::filesystem;
namespace {
constexpr const char* kTestDir = "test_dir";
}  // namespace

class FileLogSinkTest : public ::testing::Test {
   protected:
    void SetUp() override {
        fs::remove_all(kTestDir);
        EXPECT_TRUE(fs::create_directory(kTestDir));
        EXPECT_TRUE(fs::is_directory(kTestDir));

        test_file_ = std::string(kTestDir) + "/" + "test_file.txt";
    }

    void TearDown() override {
        EXPECT_TRUE(fs::is_directory(kTestDir));
        EXPECT_TRUE(fs::remove_all(kTestDir));
    }

   protected:
    std::string test_file_;
};

TEST_F(FileLogSinkTest, ConstructorWithEmptyFilename) {
    vne::log::FileLogSink file_sync("");
}

TEST_F(FileLogSinkTest, ConstructorCreatesFile) {
    log::FileLogSink file_sync(test_file_);
    EXPECT_TRUE(std::filesystem::exists(test_file_));
}

TEST_F(FileLogSinkTest, LogWritesToFile) {
    std::string unique_file = std::string(kTestDir) + "/" + "log_writes_test.txt";
    {
        vne::log::FileLogSink file_sync(unique_file);
        file_sync
            .log("LogWritesToFile", vne::log::LogLevel::eInfo, log::TimeStampType::eLocal, "Test message", "TestFile", "TestFunction", 42);
        file_sync.flush();
    }
    std::ifstream infile(unique_file);
    std::string line;
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find("Test message"), std::string::npos);
    infile.close();
    fs::remove(unique_file);
}

TEST_F(FileLogSinkTest, AppendModeDoesNotOverwriteFile) {
    {
        vne::log::FileLogSink file_sink(test_file_);
        file_sink.log("AppendModeDoesNotOverwriteFile",
                      vne::log::LogLevel::eInfo,
                      log::TimeStampType::eLocal,
                      "First message",
                      "TestFile",
                      "TestFunction",
                      42);
    }
    {
        vne::log::FileLogSink file_sink(test_file_, true);
        file_sink.log("AppendModeDoesNotOverwriteFile",
                      vne::log::LogLevel::eInfo,
                      log::TimeStampType::eLocal,
                      "Second message",
                      "TestFile",
                      "TestFunction",
                      43);
    }

    std::ifstream infile(test_file_);
    std::string line;
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find("First message"), std::string::npos);
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find("Second message"), std::string::npos);
}

TEST_F(FileLogSinkTest, NonAppendModeOverwritesFile) {
    {
        vne::log::FileLogSink file_sink(test_file_);
        file_sink.log("NonAppendModeOverwritesFile",
                      vne::log::LogLevel::eInfo,
                      log::TimeStampType::eLocal,
                      "First message",
                      "TestFile",
                      "TestFunction",
                      42);
    }
    {
        vne::log::FileLogSink file_sink(test_file_, false);
        file_sink.log("NonAppendModeOverwritesFile",
                      vne::log::LogLevel::eInfo,
                      log::TimeStampType::eLocal,
                      "Second message",
                      "TestFile",
                      "TestFunction",
                      43);
    }

    std::ifstream infile(test_file_);
    std::string line;
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find("Second message"), std::string::npos);
    EXPECT_FALSE(std::getline(infile, line));  // Only one line should be present
}

TEST_F(FileLogSinkTest, FlushWritesToFile) {
    vne::log::FileLogSink file_sink(test_file_);
    file_sink.log("FlushWritesToFile", vne::log::LogLevel::eInfo, log::TimeStampType::eLocal, "Test message", "TestFile", "TestFunction", 42);

    // Before flush, file should be empty or not contain the message
    std::ifstream infile_before(test_file_);
    std::string line_before;
    bool found_before = false;
    while (std::getline(infile_before, line_before)) {
        if (line_before.find("Test message") != std::string::npos) {
            found_before = true;
            break;
        }
    }
    infile_before.close();
    EXPECT_FALSE(found_before) << "Message should not be present before flush.";

    // Now flush and check for the message, retry a few times for robustness
    file_sink.flush();
    bool found_after = false;
    for (int attempt = 0; attempt < 3 && !found_after; ++attempt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::ifstream infile(test_file_);
        std::string line;
        while (std::getline(infile, line)) {
            if (line.find("Test message") != std::string::npos) {
                found_after = true;
                break;
            }
        }
        infile.close();
    }
    EXPECT_TRUE(found_after) << "Message should be present after flush.";
}

TEST_F(FileLogSinkTest, SetPatternChangesLogFormat) {
    std::string unique_file = std::string(kTestDir) + "/" + "set_pattern_test.txt";
    {
        vne::log::FileLogSink file_sink(unique_file);
        std::string customPattern = "[%l] %v";
        file_sink.setPattern(customPattern);
        file_sink.log("SetPatternChangesLogFormat",
                      vne::log::LogLevel::eInfo,
                      log::TimeStampType::eLocal,
                      "Test message",
                      "TestFile",
                      "TestFunction",
                      42);
        file_sink.flush();
    }
    std::ifstream infile(unique_file);
    std::string line;
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find("[INFO] Test message"), std::string::npos);
    infile.close();
    fs::remove(unique_file);
}

TEST_F(FileLogSinkTest, GetPatternReturnsCurrentPattern) {
    vne::log::FileLogSink file_sink(test_file_);
    std::string customPattern = "[%l] %v";
    file_sink.setPattern(customPattern);
    EXPECT_EQ(file_sink.getPattern(), customPattern);
}

TEST_F(FileLogSinkTest, GetFileName) {
    vne::log::FileLogSink file_sink(test_file_);
    EXPECT_EQ(file_sink.getFileName(), test_file_);
}

TEST_F(FileLogSinkTest, IsAppend) {
    {
        vne::log::FileLogSink file_sink(test_file_);
        EXPECT_TRUE(file_sink.isAppend());
    }
    {
        vne::log::FileLogSink file_sink(test_file_, false);
        EXPECT_FALSE(file_sink.isAppend());
    }
}

TEST_F(FileLogSinkTest, GetLogSink) {
    vne::log::FileLogSink file_sink(test_file_);
    EXPECT_NE(file_sink.clone(), nullptr);
}
