# DX9 Engine Compilation Fixes

## Issues Found and Fixed

### 1. Multiple Definition Error - StringToWString
**Problem:** The `StringToWString` function was defined in multiple .cpp files:
- `src/Core/Window.cpp`
- `src/Shaders/Effect.cpp`
- `src/Textures/Texture.cpp`

**Solution:**
- Created `src/Core/Utils.h` with inline `StringToWString` function
- Updated all source files to include and use the utility header instead of defining their own versions

### 2. Missing Texture Effect Source Files
**Problem:** CMakeLists.txt was missing several texture effect source files, causing linker errors for `EffectManager` methods.

**Solution:** Added all missing texture effect files to `TEXTURE_SOURCES`:
- `src/Textures/Effects/AnimatedEffects.cpp`
- `src/Textures/Effects/NoiseGenerator.cpp`
- `src/Textures/Effects/PostEffects.cpp`
- `src/Textures/Effects/ProceduralTextures.cpp`
- `src/Textures/Effects/TextureEffectManager.cpp`
- `src/Textures/Effects/TextureUtils.cpp`
- `src/Textures/Effects/UVEffects.cpp`

### 3. WinMain vs main Function Issue
**Problem:** Project uses `WinMain` but linker was configured for console application looking for `main` function.

**Solution:**
- Added `WIN32` flag to `add_executable` in CMakeLists.txt
- Added Windows subsystem linker flag for MSVC: `/SUBSYSTEM:WINDOWS`

## Files Modified

1. **Created:** `src/Core/Utils.h` - Utility functions header
2. **Modified:** `src/Core/Window.cpp` - Removed local StringToWString, added Utils.h include
3. **Modified:** `src/Shaders/Effect.cpp` - Removed local StringToWString, added Utils.h include
4. **Modified:** `src/Textures/Texture.cpp` - Removed local StringToWString, added Utils.h include
5. **Modified:** `CMakeLists.txt` - Added missing source files and Windows subsystem configuration

## Build Instructions

1. Open Visual Studio 2019/2022 or use CMake directly
2. Make sure DirectX SDK is installed in the default path: `C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)`
3. Configure and build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

## Project Structure
The DX9 Engine is a DirectX 9-based texture engine with advanced features:
- Multi-layer texture system (up to 8 textures per material)
- Animated procedural effects (lava, water, fire, plasma, etc.)
- HLSL shader system with hot-reload capability
- Real-time texture effects and UV animations

The engine demonstrates advanced DirectX 9 techniques including multi-texturing, shader programming, and real-time texture generation.
