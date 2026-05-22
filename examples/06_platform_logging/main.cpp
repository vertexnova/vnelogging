/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   May 2026
 *
 * Autodoc:   yes
 *
 * ----------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------
 * Example: Cross-platform logging setup
 *
 * Demonstrates how to initialise the logger on each supported platform.
 * Mobile and sandboxed platforms (iOS, visionOS, Android, Web) cannot
 * resolve a writable log directory on their own — the app must obtain the
 * correct path from the platform and pass it via setAppLogDirectory()
 * before calling configureLogger().
 *
 * Platform bridging snippets
 * --------------------------
 *
 * iOS / visionOS (Swift, call before your C++ init):
 *
 *   import Foundation
 *   // expose a C symbol that the Swift layer can call
 *   @_silgen_name("vne_set_log_dir")
 *   func vneSetLogDir(_ path: UnsafePointer<CChar>) {
 *       vne::log::Logging::setAppLogDirectory(String(cString: path))
 *   }
 *   // --- in AppDelegate / @main ---
 *   let dir = FileManager.default
 *       .urls(for: .documentDirectory, in: .userDomainMask)[0]
 *       .appendingPathComponent("logs").path
 *   dir.withCString { vneSetLogDir($0) }
 *
 * Android (JNI, call from your Activity / Application.onCreate):
 *
 *   extern "C" JNIEXPORT void JNICALL
 *   Java_com_example_app_MainActivity_initLogging(JNIEnv* env, jobject ctx) {
 *       jclass cls    = env->GetObjectClass(ctx);
 *       jmethodID mid = env->GetMethodID(cls, "getFilesDir", "()Ljava/io/File;");
 *       jobject file  = env->CallObjectMethod(ctx, mid);
 *       jmethodID pathMid = env->GetMethodID(env->GetObjectClass(file),
 *                                            "getAbsolutePath",
 *                                            "()Ljava/lang/String;");
 *       jstring jpath = (jstring)env->CallObjectMethod(file, pathMid);
 *       const char* path = env->GetStringUTFChars(jpath, nullptr);
 *       vne::log::Logging::setAppLogDirectory(std::string(path) + "/logs");
 *       env->ReleaseStringUTFChars(jpath, path);
 *   }
 *
 * Web (Emscripten):
 *   File logging is not supported. The logger automatically falls back to
 *   console-only output — no extra setup needed.
 *
 * ----------------------------------------------------------------------
 */

#include <filesystem>

#include <vertexnova/logging/logging.h>

namespace {
CREATE_VNE_LOGGER_CATEGORY("platform.example")
}

// ---------------------------------------------------------------------------
// Platform-specific log directory resolution
// ---------------------------------------------------------------------------
// On desktop platforms the library resolves the directory automatically.
// On mobile/web the app must call setAppLogDirectory() before this runs.
// This helper just shows what happens per platform at runtime.
static vne::log::LogSinkType chooseSink() {
#if defined(VNE_PLATFORM_WEB)
    // No filesystem — console only.
    return vne::log::LogSinkType::eConsole;
#elif defined(VNE_PLATFORM_VISIONOS) || defined(VNE_PLATFORM_ANDROID)
    // File logging is only available if the app provided a directory.
    std::string dir = vne::log::Logging::getPlatformSpecificLogDirectory();
    return dir.empty() ? vne::log::LogSinkType::eConsole : vne::log::LogSinkType::eBoth;
#else
    return vne::log::LogSinkType::eBoth;
#endif
}

int main() {
    // On iOS / visionOS / Android the calling app must have already invoked
    // vne::log::Logging::setAppLogDirectory(<writable path>) before main()
    // or before this point.  See the bridging snippets in the file header.

    std::string log_dir = vne::log::Logging::getPlatformSpecificLogDirectory();

    vne::log::LoggerConfig config = vne::log::Logging::defaultLoggerConfig();
    config.sink = chooseSink();
    if (!log_dir.empty()) {
        config.file_path = (std::filesystem::path(log_dir) / "platform_example.log").string();
    }
    config.log_level = vne::log::LogLevel::eTrace;
    config.file_append = false;  // overwrite on each launch; set true to keep history across runs

    vne::log::Logging::configureLogger(config);

    VNE_LOG_INFO << "Logging initialised";
    VNE_LOG_INFO << "Log directory: " << (log_dir.empty() ? "(console only)" : log_dir);

    VNE_LOG_TRACE << "Trace message";
    VNE_LOG_DEBUG << "Debug message";
    VNE_LOG_WARN << "Warning message";
    VNE_LOG_ERROR << "Error message";

    vne::log::Logging::shutdown();
    return 0;
}
