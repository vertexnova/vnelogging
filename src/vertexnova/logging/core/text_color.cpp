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
#include <type_traits>

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
}  // namespace

// Outer namespace
namespace vne {
// Inner namespace
namespace log {

TextColor::TextColor()
    : attributes_(DisplayAttributes::eNormal)
    , foreground_color_(FGColorCode::eDefault)
    , background_color_(BGColorCode::eDefault) {}

TextColor::~TextColor() {}

TextColor::TextColor(DisplayAttributes attr, FGColorCode fg_color, BGColorCode bg_color)
    : attributes_(attr)
    , foreground_color_(fg_color)
    , background_color_(bg_color) {}

TextColor::TextColor(const TextColor& rhs)
    : attributes_(rhs.attributes_)
    , foreground_color_(rhs.foreground_color_)
    , background_color_(rhs.background_color_) {}

TextColor& TextColor::operator=(const TextColor& rhs) {
    attributes_ = rhs.attributes_;
    foreground_color_ = rhs.foreground_color_;
    background_color_ = rhs.background_color_;
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const TextColor& text_color) {
    return stream << "\033[" << toUnderlying(text_color.attributes_) << "m" << "\033["
                  << toUnderlying(text_color.foreground_color_) << "m" << "\033["
                  << toUnderlying(text_color.background_color_) << "m";
}

}  // namespace log
}  // namespace vne
