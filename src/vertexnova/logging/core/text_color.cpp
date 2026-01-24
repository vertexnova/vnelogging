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

#include "text_color.h"

#include <atomic>
#include <cstdlib>
#include <type_traits>

#if !defined(VNE_PLATFORM_WEB) && !defined(VNE_PLATFORM_WIN)
#include <unistd.h>  // For isatty
#endif

#ifdef VNE_PLATFORM_WIN
#include <io.h>  // For _isatty
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif
#endif

namespace {

/**
 * Converts a enum class type to underlying types (int or unsigned int).
 *
 * @note
 * https://timsong-cpp.github.io/cppwp/n4659/dcl.enum#7
 *
 */
template<typename E>
constexpr typename std::underlying_type<E>::type toUnderlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

/**
 * @brief Global color enabled state.
 *
 * -1 = auto (use detection), 0 = disabled, 1 = enabled
 */
std::atomic<int> g_color_mode{-1};

/**
 * @brief Cached result of color detection.
 */
std::atomic<int> g_color_detected{-1};

/**
 * @brief Detects if the terminal supports ANSI colors.
 */
bool detectColorSupport() {
    // Web platform: no ANSI support
#ifdef VNE_PLATFORM_WEB
    return false;
#endif

    // iOS: Xcode console doesn't support ANSI colors
#ifdef VNE_PLATFORM_IOS
    return false;
#endif

    // Check FORCE_COLOR environment variable (overrides all)
    if (const char* force_color = std::getenv("FORCE_COLOR")) {
        return force_color[0] != '0';
    }

    // Check NO_COLOR environment variable (https://no-color.org/)
    if (std::getenv("NO_COLOR") != nullptr) {
        return false;
    }

    // Check if running in Xcode debugger (macOS)
    // Xcode sets __XCODE_BUILT_PRODUCTS_DIR_PATHS or XCODE_VERSION_ACTUAL
    if (std::getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS") != nullptr || std::getenv("XCODE_VERSION_ACTUAL") != nullptr) {
        return false;
    }

    // Check TERM environment variable
    const char* term = std::getenv("TERM");
    if (term == nullptr || std::string(term) == "dumb") {
        return false;
    }

    // Check if stdout is a TTY
#ifdef VNE_PLATFORM_WIN
    if (_isatty(STDOUT_FILENO) == 0) {
        return false;
    }
#elif !defined(VNE_PLATFORM_WEB)
    if (isatty(STDOUT_FILENO) == 0) {
        return false;
    }
#endif

    // Assume color support for recognized terminals
    return true;
}

}  // namespace

namespace vne::log {

//==============================================================================
// Color support functions
//==============================================================================

bool isColorSupported() {
    int detected = g_color_detected.load(std::memory_order_relaxed);
    if (detected < 0) {
        detected = detectColorSupport() ? 1 : 0;
        g_color_detected.store(detected, std::memory_order_relaxed);
    }
    return detected != 0;
}

void setColorEnabled(bool enabled) {
    g_color_mode.store(enabled ? 1 : 0, std::memory_order_relaxed);
}

bool isColorEnabled() {
    int mode = g_color_mode.load(std::memory_order_relaxed);
    if (mode >= 0) {
        return mode != 0;
    }
    return isColorSupported();
}

const char* getResetSequence() {
    return isColorEnabled() ? "\033[0m" : "";
}

//==============================================================================
// TextColor class implementation
//==============================================================================

TextColor::TextColor()
    : attributes_(DisplayAttributes::eNormal)
    , foreground_color_(FGColorCode::eDefault)
    , background_color_(BGColorCode::eDefault) {}

TextColor::~TextColor() = default;

TextColor::TextColor(DisplayAttributes attr, FGColorCode fg_color, BGColorCode bg_color)
    : attributes_(attr)
    , foreground_color_(fg_color)
    , background_color_(bg_color) {}

TextColor::TextColor(const TextColor& rhs)
    : attributes_(rhs.attributes_)
    , foreground_color_(rhs.foreground_color_)
    , background_color_(rhs.background_color_) {}

TextColor& TextColor::operator=(const TextColor& rhs) {
    if (this != &rhs) {
        attributes_ = rhs.attributes_;
        foreground_color_ = rhs.foreground_color_;
        background_color_ = rhs.background_color_;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const TextColor& text_color) {
    if (!isColorEnabled()) {
        return stream;  // No output when colors disabled
    }

    return stream << "\033[" << toUnderlying(text_color.attributes_) << "m"
                  << "\033[" << toUnderlying(text_color.foreground_color_) << "m"
                  << "\033[" << toUnderlying(text_color.background_color_) << "m";
}

}  // namespace vne::log
