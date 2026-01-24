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

#include "log_manager.h"
#include "vertexnova/logging/core/sync_logger.h"
#include "vertexnova/logging/core/async_logger.h"
#include "vertexnova/logging/core/logger_controller.h"

namespace vne {
namespace log {

LogManager::LogManager() {}

std::shared_ptr<ILogger> LogManager::createLogger(const std::string& logger_name, bool async) {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        return it->second;
    }
    std::shared_ptr<ILogger> logger;
    if (async) {
        logger = std::make_shared<AsyncLogger>(logger_name);
    } else {
        logger = std::make_shared<SyncLogger>(logger_name);
    }
    loggers_[logger_name] = logger;
    logger_async_state_[logger_name] = async;
    LoggerController::registerLogger(logger);
    return logger;
}

bool LogManager::isLoggerAsync(const std::string& logger_name) const {
    auto it = logger_async_state_.find(logger_name);
    if (it != logger_async_state_.end()) {
        return it->second;
    }
    return false;  // Default to sync if logger doesn't exist
}

std::shared_ptr<ILogger> LogManager::getLogger(const std::string& logger_name) const {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        return it->second;
    }
    return nullptr;
}

void LogManager::addConsoleSink(const std::string& logger_name) {
    auto logger = getLogger(logger_name);
    if (logger) {
        auto console_sink = std::make_unique<ConsoleLogSink>();
        logger->addLogSink(std::move(console_sink));
    }
}

void LogManager::addFileSink(const std::string& logger_name, const std::string& log_file_path) {
    auto logger = getLogger(logger_name);
    if (logger) {
        auto file_sink = std::make_unique<FileLogSink>(log_file_path);
        logger->addLogSink(std::move(file_sink));
    }
}

void LogManager::setConsolePattern(const std::string& logger_name, const std::string& pattern) {
    auto logger = getLogger(logger_name);
    if (logger) {
        for (auto& sink : logger->getLogSinks()) {
            auto console_sink = dynamic_cast<ConsoleLogSink*>(sink.get());
            if (console_sink) {
                console_sink->setPattern(pattern);
            }
        }
    }
}

void LogManager::setFilePattern(const std::string& logger_name, const std::string& pattern) {
    auto logger = getLogger(logger_name);
    if (logger) {
        for (auto& sink : logger->getLogSinks()) {
            auto file_sink = dynamic_cast<FileLogSink*>(sink.get());
            if (file_sink) {
                file_sink->setPattern(pattern);
            }
        }
    }
}

void LogManager::setLogLevel(const std::string& logger_name, LogLevel level) {
    auto logger = getLogger(logger_name);
    if (logger) {
        logger->setCurrentLogLevel(level);
    }
}

void LogManager::setFlushLevel(const std::string& logger_name, LogLevel level) {
    auto logger = getLogger(logger_name);
    if (logger) {
        logger->setFlushLevel(level);
    }
}

void LogManager::finalize() {
    // Unregister all loggers
    for (const auto& logger_pair : loggers_) {
        logger_pair.second->flush();  // Ensure all loggers are flushed before unregistering
        LoggerController::unregisterLogger(logger_pair.first);
    }
    // Clear all logger maps
    loggers_.clear();
    logger_async_state_.clear();
}

}  // namespace log
}  // namespace vne
