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

#include <gtest/gtest.h>
#include "vertexnova/logging/core/text_color.h"

using namespace vne;

namespace {
// Helper function to capture output stream
std::string captureStream(const log::TextColor& text_color) {
    std::ostringstream stream;
    stream << text_color;
    return stream.str();
}

}  // namespace

class TextColorTest : public ::testing::Test {};

// Test default constructor
TEST_F(TextColorTest, DefaultConstructor) {
    log::TextColor text_color;
    EXPECT_EQ(captureStream(text_color), "\033[0m\033[39m\033[49m");
}

// Test parametric constructor
TEST_F(TextColorTest, ParametricConstructor) {
    log::TextColor text_color(log::DisplayAttributes::eBold, log::FGColorCode::eRed, log::BGColorCode::eBlack);
    EXPECT_EQ(captureStream(text_color), "\033[1m\033[31m\033[40m");
}

// Test copy constructor
TEST_F(TextColorTest, CopyConstructor) {
    log::TextColor original(log::DisplayAttributes::eUnderline, log::FGColorCode::eGreen, log::BGColorCode::eYellow);
    log::TextColor copy(original);
    EXPECT_EQ(captureStream(copy), "\033[4m\033[32m\033[43m");
}

// Test assignment operator
TEST_F(TextColorTest, AssignmentOperator) {
    log::TextColor original(log::DisplayAttributes::eBlink, log::FGColorCode::eBlue, log::BGColorCode::eMagenta);
    log::TextColor assigned;
    assigned = original;
    EXPECT_EQ(captureStream(assigned), "\033[5m\033[34m\033[45m");
}

// Test destructor (not directly testable, but ensuring no side-effects)
TEST_F(TextColorTest, Destructor) {
    log::TextColor* text_color =
        new log::TextColor(log::DisplayAttributes::eReverse, log::FGColorCode::eCyan, log::BGColorCode::eLightRed);
    EXPECT_EQ(captureStream(*text_color), "\033[7m\033[36m\033[101m");
    delete text_color;
}

// Test stream operator with various color combinations
TEST_F(TextColorTest, StreamOperator) {
    log::TextColor text_color(log::DisplayAttributes::eItalic,
                              log::FGColorCode::eLightBlue,
                              log::BGColorCode::eDarkGray);
    EXPECT_EQ(captureStream(text_color), "\033[3m\033[94m\033[100m");

    text_color =
        log::TextColor(log::DisplayAttributes::eHidden, log::FGColorCode::eLightMagenta, log::BGColorCode::eLightCyan);
    EXPECT_EQ(captureStream(text_color), "\033[8m\033[95m\033[106m");

    text_color =
        log::TextColor(log::DisplayAttributes::eFaint, log::FGColorCode::eWhite, log::BGColorCode::eLightGreen);
    EXPECT_EQ(captureStream(text_color), "\033[2m\033[97m\033[102m");
}
