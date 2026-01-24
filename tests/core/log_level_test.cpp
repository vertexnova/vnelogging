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

#include "vertexnova/logging/core/log_level.h"

#include <sstream>

using namespace vne;

TEST(LogLevelTest, LogLevelStreamOperator) {
    // Test each log level
    std::stringstream ss;

    ss << log::LogLevel::eTrace;
    EXPECT_EQ(ss.str(), "TRACE");

    ss.str("");
    ss << log::LogLevel::eDebug;
    EXPECT_EQ(ss.str(), "DEBUG");

    ss.str("");
    ss << log::LogLevel::eInfo;
    EXPECT_EQ(ss.str(), "INFO");

    ss.str("");
    ss << log::LogLevel::eWarn;
    EXPECT_EQ(ss.str(), "WARN");

    ss.str("");
    ss << log::LogLevel::eError;
    EXPECT_EQ(ss.str(), "ERROR");

    ss.str("");
    ss << log::LogLevel::eFatal;
    EXPECT_EQ(ss.str(), "FATAL");

    ss.str("");
    ss << static_cast<log::LogLevel>(-1);
    EXPECT_EQ(ss.str(), "UNKNOWN");
}
