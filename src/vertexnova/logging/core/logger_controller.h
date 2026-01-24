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
#include <vector>
#include <string>

#include "logger.h"

/**
 * @file logger_controller.h
 * @brief Definition of the LoggerController class for managing logging instances.
 *
 * This file provides the declaration of the LoggerController class,
 * which allows registration, retrieval, and management of logger instances
 * within a thread-safe environment using a singleton pattern.
 */

namespace vne::log {

/**
 * @brief The LoggerController class manages logger instances in a thread-safe manner.
 *
 * This class provides static methods to register, retrieve, and unregister
 * logger instances. It uses a singleton pattern to ensure that only one
 * logger instance is managed globally across the application.
 */
class LoggerController {
   public:
    /**
     * @brief Registers a logger instance with the controller.
     *
     * @param logger A shared pointer to the logger instance to register.
     */
    static void registerLogger(std::shared_ptr<ILogger> logger);

    /**
     * @brief Unregisters a logger instance by its name.
     *
     * @param logger_name The name of the logger instance to unregister.
     */
    static void unregisterLogger(const std::string& logger_name);

    /**
     * @brief Unregisters all registered loggers.
     */
    static void unregisterAllLoggers();

    /**
     * @brief Retrieves a logger instance by its name.
     *
     * @param logger_name The name of the logger instance to retrieve.
     * @return A shared pointer to the logger instance, or nullptr if not found.
     */
    static std::shared_ptr<ILogger> getLogger(const std::string& logger_name);

    /**
     * @brief Retrieves the names of all registered loggers.
     *
     * @return A vector of strings containing the names of all registered loggers.
     */
    static std::vector<std::string> getLoggerNames();

   private:
    /**
     * @brief Deleted constructor to prevent instantiation of LoggerController objects.
     *
     * The LoggerController class is designed with static methods,
     * and therefore should not be instantiated.
     */
    LoggerController() = delete;

    /**
     * @brief Retrieves the registry of logger instances.
     *
     * @return A reference to the vector holding shared pointers to logger instances.
     */
    static std::vector<std::shared_ptr<ILogger>>& getRegistry();

    /**
     * @brief Retrieves the static mutex for thread-safe operations.
     *
     * This method provides access to the static mutex used to synchronize
     * access to the logger instances.
     *
     * @return A reference to the static mutex instance.
     */
    static std::mutex& getMutex();
};

}  // namespace vne::log
