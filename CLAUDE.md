# CLAUDE.md — oclLib

## Project Overview

**oclLib** is a C/C++ library designed to simplify the use of OpenCL 1.2 for GPGPU (General-Purpose computing on Graphics Processing Units) programming. It provides higher-level abstractions over the raw OpenCL 1.2 C API, reducing boilerplate and common pitfalls for application developers.

- **License**: CC0 1.0 Universal (public domain)
- **Target API**: OpenCL 1.2
- **Language**: C and/or C++

## Repository State

As of the initial commit, the repository contains only scaffolding:

```
oclLib/
├── .gitignore      # C/C++ build artifact ignore rules
├── LICENSE         # CC0 1.0 Universal
└── README.md       # Brief project description
```

Source code has not yet been added. This CLAUDE.md will be updated as the codebase grows.

## Expected Project Structure

For a typical OpenCL helper library, the anticipated layout is:

```
oclLib/
├── include/            # Public header files (.h / .hpp)
│   └── oclLib/
│       ├── platform.h  # Platform/device enumeration
│       ├── context.h   # Context management
│       ├── queue.h     # Command queue helpers
│       ├── buffer.h    # Memory buffer wrappers
│       ├── kernel.h    # Kernel compilation & dispatch
│       └── oclLib.h    # Convenience all-in-one include
├── src/                # Library implementation files (.c / .cpp)
├── examples/           # Example programs demonstrating the library
├── tests/              # Unit/integration tests
├── CMakeLists.txt      # Primary CMake build file (or Makefile)
├── .gitignore
├── LICENSE
├── README.md
└── CLAUDE.md
```

## OpenCL 1.2 Background

OpenCL 1.2 is a cross-platform standard for parallel computation on heterogeneous hardware (CPUs, GPUs, FPGAs). Key concepts relevant to oclLib:

| Concept | Description |
|---|---|
| Platform | An OpenCL implementation (e.g., NVIDIA, AMD, Intel) |
| Device | Compute unit within a platform (GPU, CPU core, etc.) |
| Context | Container for devices, memory objects, and command queues |
| Command Queue | Ordered stream of operations sent to a device |
| Program | Compiled collection of kernels |
| Kernel | Single compute function executed on a device |
| Buffer | Device-side memory object (cl_mem) |
| NDRange | Work-item grid specification for kernel dispatch |

## Build System

The `.gitignore` is configured for compiled C/C++ artifacts. The build system is not yet established. When adding one:

- **Preferred**: CMake (modern, cross-platform, supports find_package for OpenCL)
- **Alternative**: GNU Make with pkg-config

### CMake OpenCL Detection (expected pattern)

```cmake
cmake_minimum_required(VERSION 3.10)
project(oclLib C CXX)

find_package(OpenCL REQUIRED)

add_library(oclLib STATIC
    src/platform.c
    src/context.c
    # ...
)

target_include_directories(oclLib PUBLIC include)
target_link_libraries(oclLib PUBLIC OpenCL::OpenCL)
```

### Build Commands (once CMake is added)

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Development Workflows

### Setting Up

1. Install OpenCL headers and ICD loader:
   - Debian/Ubuntu: `sudo apt install opencl-headers ocl-icd-opencl-dev`
   - Fedora/RHEL: `sudo dnf install opencl-headers ocl-icd-devel`
   - macOS: OpenCL is provided by the OS (deprecated since macOS 10.14)
2. Install a vendor OpenCL runtime:
   - NVIDIA GPUs: CUDA toolkit includes the OpenCL ICD
   - AMD GPUs: ROCm or AMDGPU-PRO
   - Intel CPUs/GPUs: Intel OpenCL Runtime or oneAPI
   - Any hardware (CPU fallback): `pocl` (Portable Computing Language)

### Running Tests (once tests exist)

```bash
cd build
ctest --output-on-failure
```

### Checking Available OpenCL Platforms (diagnostic)

```bash
# Install clinfo if available
clinfo
```

## Coding Conventions

### Language & Style

- Use **C99** for maximum portability in core library code; C++11 or later for C++ wrappers if provided
- Header guards: use `#ifndef OCLLIB_<FILENAME>_H` / `#define` / `#endif` pattern (or `#pragma once`)
- Naming:
  - Types/structs: `ocl_<name>_t` (C) or `ocl::<Name>` (C++ namespace)
  - Functions: `ocl_<noun>_<verb>` (e.g., `ocl_context_create`, `ocl_buffer_write`)
  - Macros: `OCLLIB_<NAME>` in all caps
- Indent with 4 spaces; no tabs
- Line length: 100 characters maximum

### Error Handling

OpenCL functions return `cl_int` error codes. The library should:

- Check every OpenCL return code
- Provide a helper macro or function to stringify `cl_int` errors
- Return errors to callers rather than aborting (library code must not call `exit()`)

```c
// Example pattern
#define OCL_CHECK(err) ocl_check_error((err), __FILE__, __LINE__)

static inline cl_int ocl_check_error(cl_int err, const char *file, int line) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "OpenCL error %d at %s:%d\n", err, file, line);
    }
    return err;
}
```

### Memory Management

- Always release OpenCL objects with their corresponding `clRelease*` functions
- Document ownership semantics clearly in headers
- Prefer RAII wrappers in C++ code to ensure cleanup

### OpenCL 1.2 Compatibility

- Do **not** use APIs introduced after OpenCL 1.2 (e.g., `clCreateCommandQueueWithProperties` is 2.0+)
- Use `clCreateCommandQueue` (deprecated in 2.0, but correct for 1.2 targets)
- Define `CL_TARGET_OPENCL_VERSION 120` before including `<CL/cl.h>` to suppress deprecation warnings and prevent use of post-1.2 APIs

```c
#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
```

## Key Files for AI Assistants

| File/Directory | Purpose |
|---|---|
| `README.md` | User-facing project description |
| `CLAUDE.md` | This file — context for AI assistants |
| `include/` | Public API surface; changes here affect all consumers |
| `src/` | Implementation details; internal changes preferred here |
| `examples/` | Usage demonstrations; keep simple and well-commented |
| `tests/` | Correctness verification; run before any commit |
| `CMakeLists.txt` | Build configuration; keep OpenCL detection robust |

## Guidelines for AI Assistants

1. **Read before modifying**: Always read existing files before proposing edits.
2. **OpenCL 1.2 only**: Do not introduce OpenCL 2.x or 3.x APIs. Check the target version (`CL_TARGET_OPENCL_VERSION 120`).
3. **No dynamic allocation surprises**: Clearly document who allocates and who frees each resource.
4. **Cross-platform**: Avoid platform-specific assumptions (Windows, Linux, macOS all use OpenCL via ICDs). Use `CL/cl.h` not `OpenCL/cl.h` (the latter is macOS-specific).
5. **CC0 license**: All contributions should be compatible with public domain dedication. Do not introduce code under copyleft licenses.
6. **Minimal dependencies**: This is a thin wrapper library; avoid pulling in heavy dependencies. Link only against the OpenCL ICD loader.
7. **Update this file**: When significant structural changes are made (new directories, build system changes, new conventions), update CLAUDE.md accordingly.

## Ignored Build Artifacts

The `.gitignore` excludes:

- Compiled object files (`*.o`, `*.obj`, `*.ko`, `*.elf`)
- Static and shared libraries (`*.a`, `*.lib`, `*.so`, `*.dll`, `*.dylib`)
- Executables (`*.exe`, `*.out`, `*.app`)
- Linker outputs (`*.map`, `*.exp`, `*.ilk`)
- Precompiled headers (`*.gch`, `*.pch`)
- Debug info (`*.dSYM/`, `*.pdb`, `*.idb`, `*.dwo`)
- Dependency files (`*.d`)
- Kernel module artifacts (`*.mod*`, `*.cmd`, `modules.order`, `Module.symvers`)
