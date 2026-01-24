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

#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>

namespace vne::log {

/**
 * @enum TimeStampType
 * @brief Specifies the type of timestamp to generate.
 */
enum class TimeStampType {
    eLocal = 0,  //!< Local time
    eUtc = 1     //!< Coordinated Universal Time (UTC)
};

/**
 * @class ITimeProvider
 * @brief Interface for providing the current time.
 */
class ITimeProvider {
   public:
    virtual ~ITimeProvider() = default;

    /**
     * @brief Get the current time as a time_t object.
     * @return The current time.
     */
    virtual std::time_t now() const = 0;

    /**
     * @brief Get the local time from a time_t object.
     * @param time The time_t object representing the current time.
     * @return A pointer to a tm structure representing the local time.
     */
    virtual std::tm* localTime(const std::time_t* time) const = 0;

    /**
     * @brief Get the UTC time from a time_t object.
     * @param time The time_t object representing the current time.
     * @return A pointer to a tm structure representing the UTC time.
     */
    virtual std::tm* gmTime(const std::time_t* time) const = 0;
};

/**
 * @class TimeProvider
 * @brief Default implementation of ITimeProvider using the system clock.
 */
class TimeProvider : public ITimeProvider {
   public:
    std::time_t now() const override { return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }

    std::tm* localTime(const std::time_t* time) const override {
        thread_local std::tm result;
#ifdef _WIN32
        localtime_s(&result, time);
#else
        localtime_r(time, &result);
#endif
        return &result;
    }

    std::tm* gmTime(const std::time_t* time) const override {
        thread_local std::tm result;
#ifdef _WIN32
        gmtime_s(&result, time);
#else
        gmtime_r(time, &result);
#endif
        return &result;
    }
};

/**
 * @class TimeStamp
 * @brief Generates timestamps based on the specified type (local or UTC).
 */
class TimeStamp {
   public:
    /**
     * @brief Constructor for TimeStamp.
     * @param type The type of timestamp to generate (local or UTC).
     * @param provider A shared pointer to a ITimeProvider instance.
     */
    TimeStamp(TimeStampType type = TimeStampType::eLocal,
              std::shared_ptr<ITimeProvider> provider = std::make_shared<TimeProvider>())
        : type_(type)
        , provider_(std::move(provider)) {}

    /**
     * @brief Get the formatted timestamp as a string.
     * @return The formatted timestamp string.
     */
    std::string getTimeStamp() const {
        auto now_c = provider_->now();
        std::tm* ptm = (type_ == TimeStampType::eLocal) ? provider_->localTime(&now_c) : provider_->gmTime(&now_c);

        std::ostringstream time_stream;
        time_stream << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
        return time_stream.str();
    }

   private:
    TimeStampType type_;                       //!< The type of timestamp to generate
    std::shared_ptr<ITimeProvider> provider_;  //!< The time provider instance
};

}  // namespace vne::log
