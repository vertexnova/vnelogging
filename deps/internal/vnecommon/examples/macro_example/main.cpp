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
// Platform Detection Macros (VNE_PLATFORM_*, VNE_COMPILER_*, VNE_ARCH_*)
//==============================================================================

void printPlatformInfo() {
    std::cout << "=== Platform Detection Macros ===" << std::endl;

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

void printCompilerInfo() {
    std::cout << "=== Compiler Detection Macros ===" << std::endl;

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

void printArchitectureInfo() {
    std::cout << "=== Architecture Detection Macros ===" << std::endl;

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
// Assertion Macros (VNE_ASSERT, VNE_ASSERT_MSG)
//==============================================================================

void demonstrateAssertions() {
    std::cout << "=== Assertion Macros ===" << std::endl;

    int value = 42;

    VNE_ASSERT(value == 42);
    std::cout << "VNE_ASSERT(value == 42) passed" << std::endl;

    VNE_ASSERT_MSG(value > 0, "Value must be positive");
    std::cout << "VNE_ASSERT_MSG(value > 0, ...) passed" << std::endl;

    std::cout << std::endl;
}

//==============================================================================
// Utility Macros (VNE_UNUSED, VNE_FORCE_INLINE, VNE_LIKELY, VNE_UNLIKELY)
//==============================================================================

VNE_FORCE_INLINE int addNumbers(int a, int b) {
    return a + b;
}

void demonstrateUtilityMacros() {
    std::cout << "=== Utility Macros ===" << std::endl;

    int unusedVar = 100;
    VNE_UNUSED(unusedVar);
    std::cout << "VNE_UNUSED(unusedVar) - no compiler warning" << std::endl;

    int result = addNumbers(10, 20);
    std::cout << "VNE_FORCE_INLINE addNumbers(10, 20) = " << result << std::endl;

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
    std::cout << "VneCommon Macro Example" << std::endl;
    std::cout << "========================" << std::endl << std::endl;

    printPlatformInfo();
    printCompilerInfo();
    printArchitectureInfo();
    demonstrateAssertions();
    demonstrateUtilityMacros();

    std::cout << "Macro example completed successfully!" << std::endl;

    return 0;
}
