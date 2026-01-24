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

#include <iostream>
#include <ostream>

/**
 * @file text_color.h
 * @brief Defines the TextColor class for coloring and formatting console output.
 */

namespace vne::log {

//==============================================================================
// Color support detection
//==============================================================================

/**
 * @brief Checks if the current terminal supports ANSI color codes.
 *
 * Detects color support by checking:
 * - Platform (web/iOS don't support ANSI)
 * - Xcode debugger console (TERM_PROGRAM=Apple_Terminal, but not in Xcode)
 * - Environment variables (TERM, NO_COLOR, FORCE_COLOR)
 * - TTY status
 *
 * @return True if ANSI colors are supported, false otherwise.
 */
[[nodiscard]] bool isColorSupported();

/**
 * @brief Enables or disables color output globally.
 *
 * Overrides automatic detection. Useful for:
 * - Forcing colors off for log file parsing
 * - Forcing colors on for terminals that aren't detected properly
 *
 * @param enabled True to enable colors, false to disable.
 */
void setColorEnabled(bool enabled);

/**
 * @brief Gets the current color enabled state.
 *
 * @return True if colors are currently enabled, false otherwise.
 */
[[nodiscard]] bool isColorEnabled();

/**
 * @brief Returns the ANSI reset sequence if colors are enabled, empty string otherwise.
 *
 * @return Reset sequence "\033[0m" or empty string.
 */
[[nodiscard]] const char* getResetSequence();

/**
 * @enum FGColorCode
 * @brief Enumeration for foreground color codes.
 */
enum class FGColorCode {
    eBlack = 30,         //!< Black foreground color (30)
    eRed = 31,           //!< Red foreground color (31)
    eGreen = 32,         //!< Green foreground color (32)
    eYellow = 33,        //!< Yellow foreground color (33)
    eBlue = 34,          //!< Blue foreground color (34)
    eMagenta = 35,       //!< Magenta foreground color (35)
    eCyan = 36,          //!< Cyan foreground color (36)
    eLightGray = 37,     //!< Light gray foreground color (37)
    eDefault = 39,       //!< Default console foreground color (39)
    eDarkGray = 90,      //!< Dark gray foreground color (90)
    eLightRed = 91,      //!< Light red foreground color (91)
    eLightGreen = 92,    //!< Light green foreground color (92)
    eLightYellow = 93,   //!< Light yellow foreground color (93)
    eLightBlue = 94,     //!< Light blue foreground color (94)
    eLightMagenta = 95,  //!< Light magenta foreground color (95)
    eLightCyan = 96,     //!< Light cyan foreground color (96)
    eWhite = 97          //!< White foreground color (97)
};

/**
 * @enum BGColorCode
 * @brief Enumeration for background color codes.
 */
enum class BGColorCode {
    eBlack = 40,          //!< Black background color (40)
    eRed = 41,            //!< Red background color (41)
    eGreen = 42,          //!< Green background color (42)
    eYellow = 43,         //!< Yellow background color (43)
    eBlue = 44,           //!< Blue background color (44)
    eMagenta = 45,        //!< Magenta background color (45)
    eCyan = 46,           //!< Cyan background color (46)
    eLightGray = 47,      //!< Light gray background color (47)
    eDefault = 49,        //!< Default console background color (49)
    eDarkGray = 100,      //!< Dark gray background color (100)
    eLightRed = 101,      //!< Light red background color (101)
    eLightGreen = 102,    //!< Light green background color (102)
    eLightYellow = 103,   //!< Light yellow background color (103)
    eLightBlue = 104,     //!< Light blue background color (104)
    eLightMagenta = 105,  //!< Light magenta background color (105)
    eLightCyan = 106,     //!< Light cyan background color (106)
    eWhite = 107          //!< White background color (107)
};

/**
 * @enum DisplayAttributes
 * @brief Enumeration for display attribute codes.
 */
enum class DisplayAttributes {
    eNormal = 0,     //!< Normal text (0)
    eBold = 1,       //!< Bold text (1)
    eFaint = 2,      //!< Faint text (2)
    eItalic = 3,     //!< Italic text (3)
    eUnderline = 4,  //!< Underlined text (4)
    eBlink = 5,      //!< Blinking text (5)
    eReverse = 7,    //!< Reversed text (7)
    eHidden = 8      //!< Hidden text (8)
};

/**
 * @class TextColor
 * @brief Class for setting text color and formatting in console output.
 *
 * The TextColor class supports various text formatting options and color settings
 * using ANSI escape codes. It allows for customization of text attributes, foreground
 * and background colors in console output.
 *
 * @note For more information on ANSI escape codes, see:
 * https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
 * https://misc.flogisoft.com/bash/tip_colors_and_formatting
 */
class TextColor {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes the TextColor object with default attributes and colors.
     */
    TextColor();

    /**
     * @brief Destructor.
     */
    ~TextColor();

    /**
     * @brief Parametric constructor.
     *
     * Initializes the TextColor object with the specified attributes and colors.
     *
     * @param attr Display attribute.
     * @param fg_color Foreground color.
     * @param bg_color Background color.
     */
    TextColor(DisplayAttributes attr, FGColorCode fg_color, BGColorCode bg_color);

    /**
     * @brief Copy constructor.
     *
     * Initializes the TextColor object as a copy of another TextColor object.
     *
     * @param rhs The TextColor object to copy.
     */
    TextColor(const TextColor& rhs);

    /**
     * @brief Assignment operator.
     *
     * Assigns the state of another TextColor object to this object.
     *
     * @param rhs The TextColor object to assign from.
     * @return A reference to this object.
     */
    TextColor& operator=(const TextColor& rhs);

    /**
     * @brief Stream operator for outputting the TextColor object.
     *
     * Formats the output stream with the text color and attributes.
     *
     * @param stream The output stream.
     * @param text_color The TextColor object.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& stream, const TextColor& text_color);

   private:
    DisplayAttributes attributes_;  //!< Display attribute
    FGColorCode foreground_color_;  //!< Foreground color
    BGColorCode background_color_;  //!< Background color
};

}  // namespace vne::log
