/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   MAY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#if BUILD_SPDLOG
#include "vertexnova/logging/log.h"

#include <gtest/gtest.h>
#include <type_traits>

namespace TestLog {
TEST(LogTest, PrintLogs) {
    vne::log::init();
    LOG_TRACE("TRACE Log Test");
    LOG_INFO("INFO Log Test");
    LOG_WARN("WARN Log Test");
    LOG_ERROR("ERROR Log Test");
    LOG_CRITICAL("CRITICAL Log Test");
}
}  // namespace TestLog

#endif
