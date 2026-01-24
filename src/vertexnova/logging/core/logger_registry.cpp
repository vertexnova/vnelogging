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

#include "logger_registry.h"
#include <algorithm>

namespace vne {
namespace log {

thread_local std::shared_ptr<ILogger> LoggerRegistry::current_logger_ = nullptr;

std::unordered_map<std::string, std::shared_ptr<ILogger>>& LoggerRegistry::registry() {
    static std::unordered_map<std::string, std::shared_ptr<ILogger>> registry;
    return registry;
}

std::mutex& LoggerRegistry::mutex() {
    static std::mutex mtx;
    return mtx;
}

void LoggerRegistry::registerLogger(const std::string& name, std::shared_ptr<ILogger> logger) {
    if (!logger)
        return;
    std::lock_guard<std::mutex> lock(mutex());
    registry()[name] = logger;
    current_logger_ = logger;
}

void LoggerRegistry::unregisterLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex());
    registry().erase(name);
}

void LoggerRegistry::unregisterAll() {
    std::lock_guard<std::mutex> lock(mutex());
    registry().clear();
    current_logger_.reset();
}

std::shared_ptr<ILogger> LoggerRegistry::getLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = registry().find(name);
    return (it != registry().end()) ? it->second : nullptr;
}

std::vector<std::string> LoggerRegistry::getLoggerNames() {
    std::lock_guard<std::mutex> lock(mutex());
    std::vector<std::string> names;
    for (const auto& pair : registry())
        names.push_back(pair.first);
    return names;
}

void LoggerRegistry::setCurrentLogger(const std::string& name) {
    current_logger_ = getLogger(name);
}

std::shared_ptr<ILogger> LoggerRegistry::getCurrentLogger() {
    return current_logger_;
}

}  // namespace log
}  // namespace vne
