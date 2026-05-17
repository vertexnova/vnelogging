#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   MAY-2026
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include <ios>
#include <iostream>
#include <streambuf>

// Redirects cout/cerr to a string stream for capturing console output.
class StreamRedirect {
   public:
    StreamRedirect(std::ios& stream, std::streambuf* new_buffer)
        : stream_(stream)
        , old_(stream.rdbuf(new_buffer)) {}

    ~StreamRedirect() { stream_.rdbuf(old_); }

    StreamRedirect(const StreamRedirect&) = delete;
    StreamRedirect& operator=(const StreamRedirect&) = delete;

   private:
    std::ios& stream_;
    std::streambuf* old_;
};
