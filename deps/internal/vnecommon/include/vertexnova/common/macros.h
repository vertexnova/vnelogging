#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2025
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include <iostream>

//==============================================================================
// Assertions
//==============================================================================

/**
 * @def VNE_ASSERT_MSG(condition, message)
 * @brief Asserts a condition and terminates the program with an error message
 *        if the condition is false (only in debug mode).
 * @param condition The condition to check.
 * @param message The message to display if the assertion fails.
 */
#ifdef VNE_DEVELOPER_BUILD
#define VNE_ASSERT_MSG(condition, message)                                                 \
    do {                                                                                   \
        if (!(condition)) {                                                                \
            std::cerr << "Assertion failed: (" #condition "), function " << __FUNCTION__   \
                      << ", file " << __FILE__ << ", line " << __LINE__ << "." << std::endl \
                      << "Message: " << message << std::endl;                              \
            std::terminate();                                                              \
        }                                                                                  \
    } while (false)

#define VNE_ASSERT(condition)                                                              \
    do {                                                                                   \
        if (!(condition)) {                                                                \
            std::cerr << "Assertion failed: (" #condition "), function " << __FUNCTION__   \
                      << ", file " << __FILE__ << ", line " << __LINE__ << "." << std::endl; \
            std::terminate();                                                              \
        }                                                                                  \
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
 * @def VNE_STATIC_ASSERT(condition, message)
 * @brief Compile-time assertion with a descriptive message.
 *        Use for type traits, memory layout validation, and template constraints.
 * @param condition The compile-time condition to check.
 * @param message The message to display if the assertion fails.
 *
 * @example
 * VNE_STATIC_ASSERT(sizeof(Vec4f) == 16, "Vec4f must be 16 bytes");
 * VNE_STATIC_ASSERT(std::is_trivially_copyable_v<Mat4f>, "Mat4f must be trivially copyable");
 */
#define VNE_STATIC_ASSERT(condition, message) static_assert(condition, message)

/**
 * @def VNE_STATIC_ASSERT_EXPR(condition)
 * @brief Compile-time assertion without a message (uses condition as message).
 * @param condition The compile-time condition to check.
 */
#define VNE_STATIC_ASSERT_EXPR(condition) static_assert(condition, #condition)

//==============================================================================
// Utility Macros
//==============================================================================

/**
 * @def VNE_UNUSED(x)
 * @brief Marks a variable as unused to avoid compiler warnings.
 * @param x The variable to mark as unused.
 */
#define VNE_UNUSED(x) (void)(x)

/**
 * @def VNE_ENSURE_MSG(condition, message)
 * @brief Ensures a condition is met and returns false with a message.
 * @param condition The condition to check.
 * @param message The message to display if the condition is false.
 */
#define VNE_ENSURE_MSG(condition, message)                                                 \
    do {                                                                                   \
        if (!(condition)) {                                                                \
            std::cerr << "Ensure failed: (" #condition "), function " << __FUNCTION__      \
                      << ", file " << __FILE__ << ", line " << __LINE__ << "." << std::endl \
                      << "Message: " << message << std::endl;                              \
            return false;                                                                  \
        }                                                                                  \
    } while (false)

/**
 * @def VNE_ENSURE(condition)
 * @brief Ensures a condition is met and returns false without a message.
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

//==============================================================================
// Compiler Hints
//==============================================================================

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
