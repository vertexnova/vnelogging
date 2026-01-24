/**
 * @file path_utils.h
 *
 * @brief Implementation of utility functions of the project directory paths.
 *
 * @author Ajeet Singh Yadav
 * Contact: yadav.ajeetsingh2020@gmail.com
 */

#pragma once

#include <string>

// config.h location: build directory during build, vertexnova/logging/ when installed
// Try relative path first (for installed case), then fallback to config.h (for build case)
#if __has_include("vertexnova/logging/config.h")
#include "vertexnova/logging/config.h"
#else
#include "config.h"
#endif

namespace vne {
namespace logging {
namespace utils {

//! Returns the absolute path of the vnelogging root directory if defined
inline const std::string GetRootDir() noexcept {
#ifdef VNE_ROOT_DIR
    return VNE_ROOT_DIR;
#else
    return std::string();
#endif  //  VNE_ROOT_DIR
}

//! Returns the absolute path of the vnelogging src directory if defined
inline const std::string GetSrcDir() noexcept {
#ifdef VNE_SRC_DIR
    return VNE_SRC_DIR;
#else
    return std::string();
#endif  // VNE_SRC_DIR
}

//! Returns the absolute path of the vnelogging include directory if defined
inline const std::string GetIncludeDir() noexcept {
#ifdef VNE_INCLUDE_DIR
    return VNE_INCLUDE_DIR;
#else
    return std::string();
#endif  // VNE_INCLUDE_DIR
}

//! Returns the absolute path of the third party tools directory if defined
inline const std::string GetThirdPartyDir() noexcept {
#ifdef VNE_THIRD_PARTY_DIR
    return VNE_THIRD_PARTY_DIR;
#else
    return std::string();
#endif  // VNE_THIRD_PARTY_DIR
}

//! Returns the absolute path of the tests directory if defined
inline const std::string GetTestsDir() noexcept {
#ifdef VNE_TESTS_DIR
    return VNE_TESTS_DIR;
#else
    return std::string();
#endif  // VNE_TESTS_DIR
}

//! Returns the absolute path of the test data directory if defined
inline const std::string GetTestDataDir() noexcept {
#ifdef VNE_TEST_DATA_DIR
    return VNE_TEST_DATA_DIR;
#else
    return std::string();
#endif  // VNE_TEST_DATA_DIR
}

//! Returns the absolute path of the current build directory if defined
inline const std::string GetBuildDir() noexcept {
#ifdef VNE_BUILD_DIR
    return VNE_BUILD_DIR;
#else
    return std::string();
#endif  // VNE_BUILD_DIR
}

}  // namespace utils
}  // namespace logging
}  // namespace vne
