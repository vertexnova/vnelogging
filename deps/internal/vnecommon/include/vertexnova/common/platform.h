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

//==============================================================================
// Platform Detection
//==============================================================================

/**
 * @def VNE_PLATFORM_WINDOWS
 * @def VNE_PLATFORM_MACOS
 * @def VNE_PLATFORM_IOS
 * @def VNE_PLATFORM_VISIONOS
 * @def VNE_PLATFORM_ANDROID
 * @def VNE_PLATFORM_LINUX
 * @def VNE_PLATFORM_WEB
 * @brief Detects the platform and defines corresponding macros.
 */
#if defined(_WIN32) || defined(_WIN64)
#ifndef VNE_PLATFORM_WINDOWS
#define VNE_PLATFORM_WINDOWS
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_VISION
#ifndef VNE_PLATFORM_VISIONOS
#define VNE_PLATFORM_VISIONOS
#endif
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#ifndef VNE_PLATFORM_IOS
#define VNE_PLATFORM_IOS
#endif
#else
#ifndef VNE_PLATFORM_MACOS
#define VNE_PLATFORM_MACOS
#endif
#endif
#elif defined(__ANDROID__)
#ifndef VNE_PLATFORM_ANDROID
#define VNE_PLATFORM_ANDROID
#endif
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

//==============================================================================
// Compiler Detection
//==============================================================================

/**
 * @def VNE_COMPILER_MSVC
 * @def VNE_COMPILER_GCC
 * @def VNE_COMPILER_CLANG
 * @brief Detects the compiler and defines corresponding macros.
 */
#if defined(_MSC_VER)
#define VNE_COMPILER_MSVC _MSC_VER
#elif defined(__clang__)
#define VNE_COMPILER_CLANG __clang_major__
#elif defined(__GNUC__)
#define VNE_COMPILER_GCC __GNUC__
#else
#error "Unknown compiler!"
#endif

//==============================================================================
// Architecture Detection
//==============================================================================

/**
 * @def VNE_ARCH_X64
 * @def VNE_ARCH_X86
 * @def VNE_ARCH_ARM64
 * @def VNE_ARCH_ARM
 * @brief Detects the CPU architecture.
 */
#if defined(__x86_64__) || defined(_M_X64)
#define VNE_ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
#define VNE_ARCH_X86
#elif defined(__aarch64__) || defined(_M_ARM64)
#define VNE_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define VNE_ARCH_ARM
#endif
