#ifndef VNE_LOGGING_UTILS_MACROS_H
#define VNE_LOGGING_UTILS_MACROS_H
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   JUL-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include <iostream>

/**
 * @def VNE_ASSERT_MSG(condition, message)
 * @brief Asserts a condition and terminates the program with an error message if the condition is false (only in debug
 * mode).
 * @param condition The condition to check.
 * @param message The message to display if the assertion fails.
 */
#ifdef VNE_DEVELOPER_BUILD
#define VNE_ASSERT_MSG(condition, message)                                                                        \
    do {                                                                                                          \
        if (!(condition)) {                                                                                       \
            std::cerr << "Assertion failed: (" #condition "), function " << __FUNCTION__ << ", file " << __FILE__ \
                      << ", line " << __LINE__ << "." << std::endl                                                \
                      << "Message: " << message << std::endl;                                                     \
            std::terminate();                                                                                     \
        }                                                                                                         \
    } while (false)
#define VNE_ASSERT(condition)                                                                                     \
    do {                                                                                                          \
        if (!(condition)) {                                                                                       \
            std::cerr << "Assertion failed: (" #condition "), function " << __FUNCTION__ << ", file " << __FILE__ \
                      << ", line " << __LINE__ << "." << std::endl;                                               \
            std::terminate();                                                                                     \
        }                                                                                                         \
    } while (false)
#else
#define VNE_ASSERT_MSG(condition, message) \
    do {                                   \
    } while (false)
#define VNE_ASSERT(condition) \
    do {                      \
    } while (false)
#endif

/**
 * @def VNE_UNUSED(x)
 * @brief Marks a variable as unused to avoid compiler warnings.
 * @param x The variable to mark as unused.
 */
#define VNE_UNUSED(x) (void)(x)

/**
 * @def VNE_ENSURE_MSG(condition)
 * @brief Ensures a condition is met and returns false, with a message.
 * @param condition The condition to check.
 * @param message The message to display if the condition is false.
 */
#define VNE_ENSURE_MSG(condition, message)                                                                     \
    do {                                                                                                       \
        if (!(condition)) {                                                                                    \
            std::cerr << "Ensure failed: (" #condition "), function " << __FUNCTION__ << ", file " << __FILE__ \
                      << ", line " << __LINE__ << "." << std::endl                                             \
                      << "Message: " << message << std::endl;                                                  \
            return false;                                                                                      \
        }                                                                                                      \
    } while (false)

/**
 * @def VNE_ENSURE(condition)
 * @brief Ensures a condition is met and returns false, without a message.
 * @param condition The condition to check.
 */
#define VNE_ENSURE(condition) \
    do {                      \
        if (!(condition)) {   \
            return false;     \
        }                     \
    } while (false)

/**
 * @def VNE_DEPRECATED
 * @brief Marks a function or variable as deprecated.
 */
#define VNE_DEPRECATED [[deprecated]]

/**
 * @def VNE_FORCE_INLINE
 * @brief Forces a function to be inlined by the compiler.
 */
#ifdef _MSC_VER
#define VNE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define VNE_FORCE_INLINE __attribute__((always_inline)) inline
#else
#define VNE_FORCE_INLINE inline
#endif

/**
 * @def VNE_LIKELY(x)
 * @def VNE_UNLIKELY(x)
 * @brief Provides hints to the compiler about the likelihood of a condition.
 * @param x The condition.
 */
#if defined(__GNUC__) || defined(__clang__)
#define VNE_LIKELY(x) __builtin_expect(!!(x), 1)
#define VNE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define VNE_LIKELY(x) (x)
#define VNE_UNLIKELY(x) (x)
#endif

/**
 * @def VNE_PLATFORM_WINDOWS
 * @def VNE_PLATFORM_MACOS
 * @def VNE_PLATFORM_IOS
 * @def VNE_PLATFORM_ANDROID
 * @def VNE_PLATFORM_LINUX
 * @def VNE_PLATFORM_WEB
 * @brief Detects the platform and defines corresponding macros for Windows, macOS, iOS, Android, Web, and Linux.
 */
#if defined(_WIN32) || defined(_WIN64)
// VNE_PLATFORM_WIN is defined by CMake
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#ifndef VNE_PLATFORM_IOS
#define VNE_PLATFORM_IOS
#endif
#else
#ifndef VNE_PLATFORM_MACOS
#define VNE_PLATFORM_MACOS
#endif
#endif
#elif defined(__ANDROID__)
#define VNE_PLATFORM_ANDROID
#elif defined(__linux__)
#ifndef VNE_PLATFORM_LINUX
#define VNE_PLATFORM_LINUX
#endif
#elif defined(__EMSCRIPTEN__)
#ifndef VNE_PLATFORM_WEB
#define VNE_PLATFORM_WEB
#endif
#else
#error "Unknown platform!"
#endif

/**
 * @def VNE_COMPILER_MSVC
 * @def VNE_COMPILER_GCC
 * @def VNE_COMPILER_CLANG
 * @brief Detects the compiler and defines corresponding macros for MSVC, GCC, and Clang.
 */
#if defined(_MSC_VER)
#define VNE_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#define VNE_COMPILER_GCC __GNUC__
#elif defined(__clang__)
#define VNE_COMPILER_CLANG __clang_major__
#else
#error "Unknown compiler!"
#endif

#endif  // VNE_LOGGING_UTILS_MACROS_H
