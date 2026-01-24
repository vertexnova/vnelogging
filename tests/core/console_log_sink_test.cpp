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

#include "vertexnova/logging/core/console_log_sink.h"

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

class ConsoleLogSinkTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Redirect std::cout to capture console output
        redirect_ = new CoutRedirect(cout_buffer_.rdbuf());
    }

    void TearDown() override { delete redirect_; }

   protected:
    std::stringstream cout_buffer_;
    CoutRedirect* redirect_;
};

TEST_F(ConsoleLogSinkTest, ConstructorSetsDefaultPattern) {
    log::ConsoleLogSink console_log_sink;
    EXPECT_EQ(console_log_sink.getPattern(), "%x [%l] %v");
}

TEST_F(ConsoleLogSinkTest, LogOutputsFormattedMessage) {
    log::ConsoleLogSink console_log_sink;

    // Iterate through all LogLevel values and log each one
    for (int i = static_cast<int>(log::LogLevel::eTrace); i <= static_cast<int>(log::LogLevel::eFatal); ++i) {
        log::LogLevel level = static_cast<log::LogLevel>(i);
        console_log_sink.log("DefualtLogger", level, log::TimeStampType::eLocal, "Test message", "TestFile", "TestFunction", 42);

        std::string output = cout_buffer_.str();
        // Check if the logged message contains the expected message part
        EXPECT_NE(output.find("Test message"), std::string::npos);

        // Check if the logged message contains the ANSI escape codes for color formatting
        switch (level) {
            case log::LogLevel::eTrace:
                EXPECT_NE(output.find("\033[37m"), std::string::npos);  // light gray
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            case log::LogLevel::eDebug:
                EXPECT_NE(output.find("\033[34m"), std::string::npos);  // blue
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            case log::LogLevel::eInfo:
                EXPECT_NE(output.find("\033[32m"), std::string::npos);  // green
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            case log::LogLevel::eWarn:
                EXPECT_NE(output.find("\033[33m"), std::string::npos);  // yellow
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            case log::LogLevel::eError:
                EXPECT_NE(output.find("\033[31m"), std::string::npos);  // red
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            case log::LogLevel::eFatal:
                EXPECT_NE(output.find("\033[35m"), std::string::npos);  // magenta
                EXPECT_NE(output.find("\033[0m"), std::string::npos);   // Default color reset
                break;
            default:
                // Unexpected log level, fail the test
                FAIL() << "Unexpected log level";
                break;
        }

        // Clear the stringstream buffer for the next log message
        cout_buffer_.str("");
        cout_buffer_.clear();
    }
}

TEST_F(ConsoleLogSinkTest, SetPatternChangesLogFormat) {
    log::ConsoleLogSink console_log_sink;
    console_log_sink.setPattern("%v [%x] [%l] %!");
    console_log_sink
        .log("PatternChangeLogger", log::LogLevel::eInfo, log::TimeStampType::eUtc, "Test message", "TestFile", "TestFunction", 42);

    std::string output = cout_buffer_.str();
    EXPECT_NE(output.find("Test message"), std::string::npos);
    EXPECT_NE(output.find("INFO"), std::string::npos);
    EXPECT_NE(output.find("TestFunction"), std::string::npos);
}

TEST_F(ConsoleLogSinkTest, FlushDoesNotThrow) {
    log::ConsoleLogSink console_log_sink;
    EXPECT_NO_THROW(console_log_sink.flush());
}

TEST_F(ConsoleLogSinkTest, SetPatternUpdatesPattern) {
    vne::log::ConsoleLogSink console_log_sink;
    std::string new_pattern = "%v [%l] %x";

    console_log_sink.setPattern(new_pattern);
    EXPECT_EQ(console_log_sink.getPattern(), new_pattern);
}

TEST_F(ConsoleLogSinkTest, GetLogSink) {
    vne::log::ConsoleLogSink console_log_sink;
    EXPECT_NE(console_log_sink.clone(), nullptr);
}
