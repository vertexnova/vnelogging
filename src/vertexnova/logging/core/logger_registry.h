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

#include <memory>
#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>

#include "logger.h"

/**
 * @file logger_registry.h
 * @brief Definition of the LoggerRegistry class for managing logging instances.
 *
 * This file provides the declaration of the LoggerRegistry class,
 * which allows registration, retrieval, and management of logger instances
 * within a thread-safe environment using a singleton pattern.
 */

namespace vne::log {

/**
 * @class LoggerRegistry
 * @brief Thread-safe singleton registry for named logger instances.
 *
 * This class provides static methods to register, retrieve, and manage named loggers
 * in a thread-safe manner. It also supports per-thread current logger selection.
 * All logger operations are protected by a mutex for thread safety, and the registry
 * is implemented as a static unordered_map. Intended for internal use by the logging system.
 */
class LoggerRegistry {
   public:
    /**
     * @brief Register a logger by name. If the name exists, replaces the logger.
     * @param name The unique name for the logger.
     * @param logger The logger instance to register.
     */
    static void registerLogger(const std::string& name, std::shared_ptr<ILogger> logger);

    /**
     * @brief Unregister a logger by name.
     * @param name The name of the logger to remove from the registry.
     */
    static void unregisterLogger(const std::string& name);

    /**
     * @brief Unregister all loggers from the registry.
     */
    static void unregisterAll();

    /**
     * @brief Get a logger by name.
     * @param name The name of the logger to retrieve.
     * @return Shared pointer to the logger, or nullptr if not found.
     */
    static std::shared_ptr<ILogger> getLogger(const std::string& name);

    /**
     * @brief Get all registered logger names.
     * @return Vector of all logger names currently in the registry.
     */
    static std::vector<std::string> getLoggerNames();

    /**
     * @brief Set the current thread-local logger by name.
     * @param name The name of the logger to set as current for this thread.
     */
    static void setCurrentLogger(const std::string& name);

    /**
     * @brief Get the current thread-local logger.
     * @return Shared pointer to the current logger for this thread, or nullptr if not set.
     */
    static std::shared_ptr<ILogger> getCurrentLogger();

   private:
    LoggerRegistry() = delete;

    /**
     * @brief Access the static registry map.
     * @return Reference to the unordered_map of logger names to logger instances.
     */
    static std::unordered_map<std::string, std::shared_ptr<ILogger>>& registry();

    /**
     * @brief Access the static mutex for thread safety.
     * @return Reference to the mutex protecting the registry.
     */
    static std::mutex& mutex();

    /**
     * @brief Thread-local pointer to the current logger for each thread.
     */
    static thread_local std::shared_ptr<ILogger> current_logger_;
};

}  // namespace vne::log
