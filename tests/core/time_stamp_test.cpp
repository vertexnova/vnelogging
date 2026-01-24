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
#include "vertexnova/logging/core/time_stamp.h"
#include "mocks/time_stamp_provider_mock.h"

using namespace vne::log;

TEST(TimeStampTest, LocalTimeStamp) {
    auto mock_provider = std::make_shared<TimeProviderMock>();
    std::time_t mock_time = 1592229296;  // June 15, 2020, 12:34:56 UTC
    std::tm mock_tm = {};
    mock_tm.tm_year = 120;  // 2020 - 1900
    mock_tm.tm_mon = 5;     // June
    mock_tm.tm_mday = 15;
    mock_tm.tm_hour = 12;
    mock_tm.tm_min = 34;
    mock_tm.tm_sec = 56;

    EXPECT_CALL(*mock_provider, now()).WillOnce(testing::Return(mock_time));
    EXPECT_CALL(*mock_provider, localTime(testing::_)).WillOnce(testing::Return(&mock_tm));

    TimeStamp timestamp_local(TimeStampType::eLocal, mock_provider);

    std::string result = timestamp_local.getTimeStamp();
    EXPECT_EQ(result, "2020-06-15 12:34:56");
}

TEST(TimeStampTest, UTCTimeStamp) {
    auto mock_provider = std::make_shared<TimeProviderMock>();
    std::time_t mock_time = 1592229296;  // June 15, 2020, 12:34:56 UTC
    std::tm mock_tm = {};
    mock_tm.tm_year = 120;  // 2020 - 1900
    mock_tm.tm_mon = 5;     // June
    mock_tm.tm_mday = 15;
    mock_tm.tm_hour = 12;
    mock_tm.tm_min = 34;
    mock_tm.tm_sec = 56;

    EXPECT_CALL(*mock_provider, now()).WillOnce(testing::Return(mock_time));
    EXPECT_CALL(*mock_provider, gmTime(testing::_)).WillOnce(testing::Return(&mock_tm));

    TimeStamp timestamp_utc(TimeStampType::eUtc, mock_provider);
    std::string result = timestamp_utc.getTimeStamp();

    EXPECT_EQ(result, "2020-06-15 12:34:56");
}

TEST(TimeStampTest, DefaultTimeStamp) {
    auto mock_provider = std::make_shared<TimeProviderMock>();
    std::time_t mock_time = 1592229296;  // June 15, 2020, 12:34:56 UTC
    std::tm mock_tm = {};
    mock_tm.tm_year = 120;  // 2020 - 1900
    mock_tm.tm_mon = 5;     // June
    mock_tm.tm_mday = 15;
    mock_tm.tm_hour = 12;
    mock_tm.tm_min = 34;
    mock_tm.tm_sec = 56;

    EXPECT_CALL(*mock_provider, now()).WillOnce(testing::Return(mock_time));
    EXPECT_CALL(*mock_provider, localTime(testing::_)).WillOnce(testing::Return(&mock_tm));

    TimeStamp timestamp_default(TimeStampType::eLocal, mock_provider);
    std::string result = timestamp_default.getTimeStamp();

    EXPECT_EQ(result, "2020-06-15 12:34:56");
}
