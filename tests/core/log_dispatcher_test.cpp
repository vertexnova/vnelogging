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

#include "vertexnova/logging/core/log_dispatcher.h"
#include "vertexnova/logging/core/log_sink.h"
#include "vertexnova/logging/core/log_formatter.h"
#include "mocks/log_sink_mock.h"

#include <memory>

using namespace vne;
class LogDispatcherTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Create some mock log sinks
        log_sink1_ = std::make_unique<log::LogSinkMock>();
        log_sink2_ = std::make_unique<log::LogSinkMock>();
        log_sinks_.push_back(std::move(log_sink1_));
        log_sinks_.push_back(std::move(log_sink2_));
    }

    void TearDown() override { log_sinks_.clear(); }

   protected:
    std::vector<std::unique_ptr<log::ILogSink>> log_sinks_;
    std::unique_ptr<log::LogSinkMock> log_sink1_;
    std::unique_ptr<log::LogSinkMock> log_sink2_;
};

TEST_F(LogDispatcherTest, TestDispatchAndFlush) {
    log::LogDispatcher dispatcher;

    // Set expectations for log method calls
    EXPECT_CALL(*dynamic_cast<log::LogSinkMock*>(log_sinks_[0].get()),
                log("Test Logger",
                    log::LogLevel::eInfo,
                    log::TimeStampType::eLocal,
                    "Test message",
                    "TestFile",
                    "TestFunction",
                    123));
    EXPECT_CALL(*dynamic_cast<log::LogSinkMock*>(log_sinks_[1].get()),
                log("Test Logger",
                    log::LogLevel::eInfo,
                    log::TimeStampType::eLocal,
                    "Test message",
                    "TestFile",
                    "TestFunction",
                    123));

    // Dispatch a log message
    dispatcher.dispatch(log_sinks_,
                        "Test Logger",
                        log::LogLevel::eInfo,
                        log::TimeStampType::eLocal,
                        "Test message",
                        "TestFile",
                        "TestFunction",
                        123);

    // Flush log sinks
    EXPECT_CALL(*dynamic_cast<log::LogSinkMock*>(log_sinks_[0].get()), flush());
    EXPECT_CALL(*dynamic_cast<log::LogSinkMock*>(log_sinks_[1].get()), flush());
    dispatcher.flush(log_sinks_);
}
