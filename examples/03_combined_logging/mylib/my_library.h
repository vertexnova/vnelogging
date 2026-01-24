#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Example library demonstrating vne::logging integration
 * ----------------------------------------------------------------------
 */

#include <string>

namespace example {

/**
 * @brief Example library class that uses vne::logging internally
 *
 * This demonstrates how a library can use vne::logging for its internal
 * logging needs, while applications using this library can use their
 * own logging system (like spdlog).
 *
 * @code
 * example::MyLibrary lib;
 * lib.initialize();
 * lib.doWork("my_operation");
 * lib.shutdown();
 * @endcode
 */
class MyLibrary {
   public:
    // Constructors and destructor
    MyLibrary() = default;
    ~MyLibrary();

    // Rule of Five - disable copy, allow move
    MyLibrary(const MyLibrary&) = delete;
    MyLibrary& operator=(const MyLibrary&) = delete;
    MyLibrary(MyLibrary&&) noexcept = default;
    MyLibrary& operator=(MyLibrary&&) noexcept = default;

    /**
     * @brief Initialize the library and its logging system
     * @return true if initialization was successful
     */
    [[nodiscard]] bool initialize();

    /**
     * @brief Perform some work and log progress
     * @param operation_name Name of the operation to perform
     */
    void doWork(const std::string& operation_name);

    /**
     * @brief Process some data and log the results
     * @param data The data to process
     */
    void processData(const std::string& data);

    /**
     * @brief Shutdown the library and cleanup logging
     */
    void shutdown();

    /**
     * @brief Check if the library is initialized
     * @return true if initialized
     */
    [[nodiscard]] bool isInitialized() const noexcept { return is_initialized_; }

   private:
    bool is_initialized_ = false;
};

}  // namespace example
