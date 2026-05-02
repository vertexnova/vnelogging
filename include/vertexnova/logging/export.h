#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

/**
 * @file export.h
 * @brief `VNE_LOGGING_API` — DLL export/import macro for shared-library builds.
 *
 * On Windows, define @c VNE_LOGGING_BUILDING_DLL when compiling the library and
 * @c VNE_LOGGING_DLL when linking the DLL from an app.
 * On Unix shared builds, @c VNE_LOGGING_BUILDING_DLL selects default visibility; otherwise empty.
 */

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#if defined(VNE_LOGGING_BUILDING_DLL)
#define VNE_LOGGING_API __declspec(dllexport)
#elif defined(VNE_LOGGING_DLL)
#define VNE_LOGGING_API __declspec(dllimport)
#else
#define VNE_LOGGING_API
#endif
#else
#if defined(VNE_LOGGING_BUILDING_DLL) && (defined(__GNUC__) || defined(__clang__))
#define VNE_LOGGING_API __attribute__((visibility("default")))
#else
#define VNE_LOGGING_API
#endif
#endif
