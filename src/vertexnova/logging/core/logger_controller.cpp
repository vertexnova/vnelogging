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

#include "logger_controller.h"

#include <algorithm>

namespace vne {
namespace log {

std::vector<std::shared_ptr<ILogger>>& LoggerController::getRegistry() {
    static std::vector<std::shared_ptr<ILogger>> registry;
    return registry;
}

std::mutex& LoggerController::getMutex() {
    static std::mutex mutex;
    return mutex;
}

void LoggerController::registerLogger(std::shared_ptr<ILogger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(getMutex());
        getRegistry().push_back(std::move(logger));
    }
}

void LoggerController::unregisterLogger(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(getMutex());
    auto& registry = getRegistry();
    auto it = std::remove_if(registry.begin(), registry.end(), [&](const auto& logger) { return logger->getName() == logger_name; });
    registry.erase(it, registry.end());
}

void LoggerController::unregisterAllLoggers() {
    std::lock_guard<std::mutex> lock(getMutex());
    getRegistry().clear();
}

std::shared_ptr<ILogger> LoggerController::getLogger(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(getMutex());
    auto it = std::find_if(getRegistry().begin(), getRegistry().end(), [&](const auto& logger) { return logger->getName() == logger_name; });
    return (it != getRegistry().end()) ? *it : nullptr;
}

std::vector<std::string> LoggerController::getLoggerNames() {
    std::lock_guard<std::mutex> lock(getMutex());
    std::vector<std::string> names;
    for (const auto& logger : getRegistry()) {
        names.push_back(logger->getName());
    }
    return names;
}

}  // namespace log
}  // namespace vne
