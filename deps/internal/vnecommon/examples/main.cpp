/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2025
 * ----------------------------------------------------------------------
 */

#include "vertexnova/common/common.h"

#include <iostream>
#include <string>

//==============================================================================
// Platform Detection Example
//==============================================================================

void printPlatformInfo() {
    std::cout << "=== Platform Detection ===" << std::endl;

#if defined(VNE_PLATFORM_WINDOWS)
    std::cout << "Platform: Windows" << std::endl;
#elif defined(VNE_PLATFORM_MACOS)
    std::cout << "Platform: macOS" << std::endl;
#elif defined(VNE_PLATFORM_IOS)
    std::cout << "Platform: iOS" << std::endl;
#elif defined(VNE_PLATFORM_VISIONOS)
    std::cout << "Platform: visionOS" << std::endl;
#elif defined(VNE_PLATFORM_ANDROID)
    std::cout << "Platform: Android" << std::endl;
#elif defined(VNE_PLATFORM_LINUX)
    std::cout << "Platform: Linux" << std::endl;
#elif defined(VNE_PLATFORM_WEB)
    std::cout << "Platform: Web (Emscripten)" << std::endl;
#else
    std::cout << "Platform: Unknown" << std::endl;
#endif

    std::cout << std::endl;
}

//==============================================================================
// Compiler Detection Example
//==============================================================================

void printCompilerInfo() {
    std::cout << "=== Compiler Detection ===" << std::endl;

#if defined(VNE_COMPILER_MSVC)
    std::cout << "Compiler: MSVC (version " << VNE_COMPILER_MSVC << ")" << std::endl;
#elif defined(VNE_COMPILER_CLANG)
    std::cout << "Compiler: Clang (version " << VNE_COMPILER_CLANG << ")" << std::endl;
#elif defined(VNE_COMPILER_GCC)
    std::cout << "Compiler: GCC (version " << VNE_COMPILER_GCC << ")" << std::endl;
#else
    std::cout << "Compiler: Unknown" << std::endl;
#endif

    std::cout << std::endl;
}

//==============================================================================
// Architecture Detection Example
//==============================================================================

void printArchitectureInfo() {
    std::cout << "=== Architecture Detection ===" << std::endl;

#if defined(VNE_ARCH_X64)
    std::cout << "Architecture: x64 (64-bit Intel/AMD)" << std::endl;
#elif defined(VNE_ARCH_X86)
    std::cout << "Architecture: x86 (32-bit Intel/AMD)" << std::endl;
#elif defined(VNE_ARCH_ARM64)
    std::cout << "Architecture: ARM64 (64-bit ARM)" << std::endl;
#elif defined(VNE_ARCH_ARM)
    std::cout << "Architecture: ARM (32-bit ARM)" << std::endl;
#else
    std::cout << "Architecture: Unknown" << std::endl;
#endif

    std::cout << std::endl;
}

//==============================================================================
// Assertion Examples
//==============================================================================

void demonstrateAssertions() {
    std::cout << "=== Assertion Macros ===" << std::endl;

    int value = 42;

    // VNE_ASSERT - simple assertion (only active in debug builds)
    VNE_ASSERT(value == 42);
    std::cout << "VNE_ASSERT(value == 42) passed" << std::endl;

    // VNE_ASSERT_MSG - assertion with message
    VNE_ASSERT_MSG(value > 0, "Value must be positive");
    std::cout << "VNE_ASSERT_MSG(value > 0, ...) passed" << std::endl;

    std::cout << std::endl;
}

//==============================================================================
// Utility Macros Example
//==============================================================================

VNE_FORCE_INLINE int addNumbers(int a, int b) {
    return a + b;
}

void demonstrateUtilityMacros() {
    std::cout << "=== Utility Macros ===" << std::endl;

    // VNE_UNUSED - suppress unused variable warnings
    int unusedVar = 100;
    VNE_UNUSED(unusedVar);
    std::cout << "VNE_UNUSED(unusedVar) - no compiler warning" << std::endl;

    // VNE_FORCE_INLINE - force inline function
    int result = addNumbers(10, 20);
    std::cout << "VNE_FORCE_INLINE addNumbers(10, 20) = " << result << std::endl;

    // VNE_LIKELY / VNE_UNLIKELY - branch prediction hints
    int x = 5;
    if (VNE_LIKELY(x > 0)) {
        std::cout << "VNE_LIKELY(x > 0) - branch hint for likely case" << std::endl;
    }

    if (VNE_UNLIKELY(x < 0)) {
        std::cout << "This won't print" << std::endl;
    } else {
        std::cout << "VNE_UNLIKELY(x < 0) - branch hint for unlikely case" << std::endl;
    }

    std::cout << std::endl;
}

//==============================================================================
// Main
//==============================================================================

int main() {
    std::cout << "VneCommon Library Examples" << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    printPlatformInfo();
    printCompilerInfo();
    printArchitectureInfo();
    demonstrateAssertions();
    demonstrateUtilityMacros();

    std::cout << "All examples completed successfully!" << std::endl;

    return 0;
}
