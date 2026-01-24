#pragma once
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

#include "vertexnova/logging/core/time_stamp.h"
#include <gmock/gmock.h>

namespace vne {
namespace log {

/**
 * @class TimeProviderMock
 * @brief Mock class for ITimeProvider.
 *
 * This mock class provides mocked methods for testing purposes. It overrides
 * the virtual methods of the ITimeProvider interface.
 */
class TimeProviderMock : public ITimeProvider {
   public:
    /**
     * @brief Mock method for getting the current time.
     * @return The mocked current time.
     */
    MOCK_METHOD(std::time_t, now, (), (const, override));

    /**
     * @brief Mock method for getting the local time from a time_t object.
     * @param time The time_t object representing the current time.
     * @return A pointer to a tm structure representing the mocked local time.
     */
    MOCK_METHOD(std::tm*, localTime, (const std::time_t*), (const, override));

    /**
     * @brief Mock method for getting the UTC time from a time_t object.
     * @param time The time_t object representing the current time.
     * @return A pointer to a tm structure representing the mocked UTC time.
     */
    MOCK_METHOD(std::tm*, gmTime, (const std::time_t*), (const, override));
};

}  // namespace log
}  // namespace vne
