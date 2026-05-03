/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2025
 * ----------------------------------------------------------------------
 */

#include "vertexnova/common/common.h"

#include <cstddef>
#include <iostream>
#include <utility>

using namespace vne::common;

//==============================================================================
// NonCopyable: movable, not copyable (e.g. unique resource)
//==============================================================================

class UniqueHandle : private NonCopyable {
public:
    explicit UniqueHandle(int id) : id_(id) {}
    int id() const { return id_; }

private:
    int id_;
};

//==============================================================================
// NonMovable: copyable, not movable (e.g. stable address)
//==============================================================================

class PinnedBuffer : private NonMovable {
public:
    explicit PinnedBuffer(std::size_t size) : size_(size) {}
    std::size_t size() const { return size_; }

private:
    std::size_t size_;
};

//==============================================================================
// NonCopyableNonMovable: neither copy nor move (e.g. singleton-like)
//==============================================================================

class AppState : private NonCopyableNonMovable {
public:
    static AppState& instance() {
        static AppState s;
        return s;
    }
    int counter() const { return counter_; }
    void increment() { ++counter_; }

private:
    AppState() = default;
    int counter_ = 0;
};

//==============================================================================
// Main
//==============================================================================

int main() {
    std::cout << "NonCopyable / NonMovable / NonCopyableNonMovable Example"
              << std::endl;
    std::cout << "========================================================"
              << std::endl << std::endl;

    // NonCopyable: can move, cannot copy
    UniqueHandle a(1);
    UniqueHandle b(2);
    // UniqueHandle c(a);        // would not compile
    b = std::move(a);            // OK
    std::cout << "UniqueHandle move: b.id() = " << b.id() << std::endl;

    // NonMovable: can copy, cannot move
    PinnedBuffer p1(64);
    PinnedBuffer p2(p1);         // OK
    // PinnedBuffer p3(std::move(p1));  // would not compile
    std::cout << "PinnedBuffer copy: p2.size() = " << p2.size() << std::endl;

    // NonCopyableNonMovable: singleton-style usage
    AppState::instance().increment();
    AppState::instance().increment();
    std::cout << "AppState::instance().counter() = "
              << AppState::instance().counter() << std::endl;

    std::cout << std::endl << "Noncopyable example completed successfully!"
              << std::endl;

    return 0;
}
