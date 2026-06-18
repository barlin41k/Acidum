# 🧪 Acidum

Acidum is a graphics engine developed in C++.

# Getting Started

> [!IMPORTANT]  
> The engine is primarily developed on Linux, so behavior on other platforms may vary. If you encounter any platform-specific bugs, please report them by opening an issue.

## Dependencies

### Must-have:
- a compiler with **C++20 support**
- **CMake 3.20** or newer
- the following libraries must be installed in the system and available to `find_package`:
  - **GLM** — a header-only C++ mathematics library for graphics software
  - **GLFW 3** — a multi-platform library for window creation, graphic contexts, and input management
  - **Vulkan SDK** — a low-overhead, cross-platform 3D graphics and compute API for high-performance rendering

*Note: some libraries is downloaded automatically via CMake `FetchContent`.*

## Build

1. Clone the repository and navigate into the directory
```bash
git clone https://github.com/barlin41k/Acidum.git
cd Acidum
```

2. Generate the build files and compile the project *(Release mode is highly recommended for performance)*
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Launch Sandbox

After building you can start your **Sandbox application**:

```bash
./build/Sandbox/Sandbox
```

## License

Distributed under the **MIT License**. See the **LICENSE** file.