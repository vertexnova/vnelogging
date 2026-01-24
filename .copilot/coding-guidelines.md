# VertexNova Coding Guidelines

**Version:** 2.0  
**Last Updated:** January 2026  
**C++ Standard:** C++17 minimum, C++20 preferred

## Naming Conventions Summary

| Construct | Style | Example |
|-----------|-------|---------|
| Classes/Structs | PascalCase | `Buffer`, `ShaderCompiler` |
| Interface classes | `I` + PascalCase | `IRenderer`, `IBuffer` |
| Enums | PascalCase | `LogSink`, `ShaderStage` |
| Enum values | `e` + PascalCase + explicit value | `eNone = 0`, `eConsole = 1` |
| Type aliases | PascalCase | `EntityId`, `BufferHandle` |
| Functions/Methods | camelCase | `initialize()`, `createBuffer()` |
| Constants | `k` + PascalCase (in anonymous namespace) | `kMaxBufferSize` |
| Private/Protected members | snake_case + `_` | `buffer_size_`, `is_initialized_` |
| Public members | snake_case | `buffer_size`, `is_initialized` |
| Local variables | snake_case | `buffer_size`, `file_path` |
| Function parameters | snake_case | `buffer_size`, `usage` |
| Static members (private) | `s_` + snake_case + `_` | `s_instance_count_` |
| Static members (public) | `s_` + snake_case | `s_instance_count` |
| Global variables | `g_` + snake_case | `g_instance`, `g_config` |
| Booleans | `is_`, `has_`, `can_`, `should_` prefix | `is_ready_`, `has_alpha_` |
| Macros | ALL_CAPS with VNE_ prefix | `VNE_ASSERT`, `VNE_PLATFORM_WINDOWS` |
| Namespaces | lowercase | `vne`, `xgl`, `xwin` |
| File names | snake_case | `shader_compiler.h` |
| Header guards | `#pragma once` | `#pragma once` |

## Types

### Classes and Structs

Use **PascalCase** - no suffixes like `_C`:

```cpp
// Classes
class ShaderCompiler { };
class MetalSurface { };
class LogManager { };

// Structs (for POD/data containers)
struct LoggerConfig { };
struct SurfaceDescriptor { };
struct Vertex { };
```

### Interface Classes

Use **`I` prefix** followed by **PascalCase**:

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render() = 0;
    virtual void present() = 0;
};

class IBuffer {
public:
    virtual ~IBuffer() = default;
    virtual void* map() = 0;
    virtual void unmap() = 0;
};
```

### Enums

Use **PascalCase** for enum names, **`e` prefix** with **PascalCase** for values, always with explicit values:

```cpp
enum class LogSink {
    eNone = 0,
    eConsole = 1,
    eFile = 2,
    eBoth = 3
};

enum class ShaderStage {
    eVertex = 0,
    eFragment = 1,
    eCompute = 2,
    eGeometry = 3
};

enum class BufferUsage {
    eNone = 0,
    eVertex = 1 << 0,
    eIndex = 1 << 1,
    eUniform = 1 << 2,
    eStorage = 1 << 3
};
```

### Type Aliases

Use **PascalCase** - no `T` prefix:

```cpp
using BufferHandle = uint32_t;
using EntityId = uint64_t;
using DeviceId = std::string;
```

## Functions and Methods

Use **camelCase** for all functions and methods:

```cpp
class Renderer {
public:
    void initialize();
    bool createBuffer(size_t size);
    void setViewport(int width, int height);
    
    int getWidth() const;
    bool isInitialized() const;

private:
    void initializeInternal();
    bool validateState();
};

// Free functions
void processVertices(const std::vector<Vertex>& vertices);
std::vector<uint32_t> compileShader(const std::string& source);
```

## Variables

### Local Variables and Parameters

Use **snake_case**:

```cpp
int window_width = 1920;
std::string shader_source;

void setViewport(int width, int height, float min_depth, float max_depth);
```

### Member Variables

```cpp
class Buffer {
public:
    // Public members: snake_case (no trailing underscore)
    size_t size;
    BufferUsage usage;

protected:
    // Protected members: snake_case with trailing underscore
    uint32_t handle_;
    Device* device_;

private:
    // Private members: snake_case with trailing underscore
    void* data_;
    bool is_initialized_;
};
```

### Boolean Variables

Use descriptive prefixes:

```cpp
class Texture {
private:
    bool is_loaded_;
    bool has_alpha_;
    bool can_resize_;
    bool should_generate_mipmaps_;
};
```

### Static and Global Variables

```cpp
class Renderer {
public:
    static int s_instance_count;           // Public static
private:
    static Device* s_device_;              // Private static
};

extern Device* g_device;                   // Global
extern bool g_is_debug_mode;
```

### Constants

Use **`k` prefix** with **PascalCase** in anonymous namespace:

```cpp
namespace {
constexpr int kMaxBufferSize = 1024 * 1024;
constexpr float kPi = 3.14159265359f;
constexpr const char* kDefaultShaderPath = "shaders/default.glsl";
}  // namespace

class Renderer {
public:
    static constexpr int kMaxTextures = 16;
    static constexpr float kDefaultFov = 60.0f;
};
```

## Namespaces

Use **lowercase**:

```cpp
namespace vne {
namespace xgl {
    class Device { };
    class Buffer { };
}
namespace xwin {
    class Window { };
}
}

// C++17 nested syntax
namespace vne::xgl::backend {
    class VulkanDevice { };
}
```

## Code Formatting

- **4 spaces** per indentation level (no tabs)
- **120 characters** maximum line length
- **LF** line endings for all platforms
- **`#pragma once`** for header guards
- **Attached braces** (opening brace on same line)
- **Left-aligned pointers**: `int* ptr;`

## Header File Structure

```cpp
#pragma once
/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0
 * ----------------------------------------------------------------------
 */

#include "vertexnova/graphics/xgl/types.h"

#include <vector>
#include <string>
#include <memory>

namespace vne::xgl {

class Device;  // Forward declaration

class MetalSurface {
public:
    MetalSurface();
    ~MetalSurface();
    
    MetalSurface(const MetalSurface&) = delete;
    MetalSurface& operator=(const MetalSurface&) = delete;
    MetalSurface(MetalSurface&&) noexcept;
    MetalSurface& operator=(MetalSurface&&) noexcept;
    
    bool initialize(const SurfaceDescriptor& desc);
    void cleanup();
    
private:
    bool is_initialized_;
    SurfaceDescriptor descriptor_;
};

}  // namespace vne::xgl
```

## Initialization

Prefer **brace initialization `{}`** and **in-class member initializers**:

```cpp
// Brace initialization
int value{42};
std::vector<int> numbers{1, 2, 3};

// In-class member initializers for defaults
class Buffer {
private:
    size_t size_;
    bool is_mapped_{false};      // Default in-class
    void* data_{nullptr};        // Default in-class
};
```

## Modern C++ Features

- Use `auto` when type is obvious from context
- Prefer range-based for loops
- Use `constexpr` for compile-time constants
- Use `nullptr` instead of `NULL` or `0`
- Use `enum class` for type-safe enums
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Mark functions `noexcept` when they don't throw
- Use `[[nodiscard]]` for functions whose return values shouldn't be ignored
- Use `std::optional<T>` for nullable returns
- Use `std::string_view` for non-owning string parameters
- Use `std::span<T>` (C++20) for non-owning array views

## Thread Safety

Document thread safety guarantees for all public APIs:

```cpp
/**
 * @brief Get the singleton instance
 * @threadsafe This function is thread-safe.
 */
LogManager& getInstance();

/**
 * @brief Configure the logger
 * @warning Not thread-safe. Must be called before any logging.
 */
void configure(const LoggerConfig& config);
```

## Error Handling Strategy

| Scenario | Approach |
|----------|----------|
| Programming error (bug) | `assert()` |
| Expected failure | Return `bool` or error code |
| Exceptional condition | Exception |
| Nullable return | `std::optional<T>` |

## Documentation

Use Doxygen-style comments for public APIs:

```cpp
/**
 * @brief Compiles GLSL source code to SPIR-V binary
 * 
 * @param source GLSL source code to compile
 * @param stage Shader stage (vertex, fragment, compute, etc.)
 * @param entry_point Entry point function name (default: "main")
 * @return SPIR-V binary data, empty on failure
 */
std::vector<uint32_t> compileGLSLToSPIRV(
    const std::string& source,
    ShaderStage stage,
    const std::string& entry_point = "main");
```

## Tool Enforcement

These guidelines are enforced by:

- **`.clang-format`** - Automatic code formatting
- **`.clang-tidy`** - Static analysis and naming conventions
- **`.editorconfig`** - Editor settings
- **Pre-commit hooks** - Run checks before commit
- **CI/CD** - Enforce on all pull requests

See the main `CODING_GUIDELINES.md` file in the repository root for the complete documentation.
