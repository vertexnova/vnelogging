#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2026 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   February 2026
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

namespace vne {
namespace common {

//! Base: no copy; move allowed. Inherit privately.
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

//! Base: no move; copy allowed. Inherit privately.
class NonMovable {
protected:
    NonMovable() = default;
    ~NonMovable() = default;

public:
    NonMovable(const NonMovable&) = default;
    NonMovable& operator=(const NonMovable&) = default;

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};

//! Base: no copy, no move. Inherit privately.
class NonCopyableNonMovable {
protected:
    NonCopyableNonMovable() = default;
    ~NonCopyableNonMovable() = default;

public:
    NonCopyableNonMovable(const NonCopyableNonMovable&) = delete;
    NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;

    NonCopyableNonMovable(NonCopyableNonMovable&&) = delete;
    NonCopyableNonMovable& operator=(NonCopyableNonMovable&&) = delete;
};

}  // namespace common
}  // namespace vne
