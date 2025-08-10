cmake_minimum_required(VERSION 3.16)
project(DX9Engine VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# DirectX SDK paths
set(DXSDK_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")

# Find DirectX 9
find_path(DirectX9_INCLUDE_DIR
    NAMES d3d9.h
    PATHS
        "${DXSDK_DIR}/Include"
    NO_DEFAULT_PATH
)

# Set library paths based on architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    set(DX_LIB_PATH "${DXSDK_DIR}/Lib/x64")
    message(STATUS "Building for 64-bit architecture")
else()
    # 32-bit
    set(DX_LIB_PATH "${DXSDK_DIR}/Lib/x86")
    message(STATUS "Building for 32-bit architecture")
endif()

find_library(DirectX9_LIBRARY
    NAMES d3d9
    PATHS
        "${DX_LIB_PATH}"
    NO_DEFAULT_PATH
)

find_library(D3DX9_LIBRARY
    NAMES d3dx9
    PATHS
        "${DX_LIB_PATH}"
    NO_DEFAULT_PATH
)

# Source files
set(CORE_SOURCES
    src/Core/Engine.cpp
    src/Core/Window.cpp
    src/Core/Timer.cpp
)

set(GRAPHICS_SOURCES
    src/Graphics/Renderer.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Camera.cpp
)

set(TEXTURE_SOURCES
    src/Textures/TextureManager.cpp
    src/Textures/Texture.cpp
    src/Textures/Material.cpp
    src/Textures/TextureEffects.cpp
    src/Textures/Effects/AnimatedEffects.cpp
    src/Textures/Effects/NoiseGenerator.cpp
    src/Textures/Effects/PostEffects.cpp
    src/Textures/Effects/ProceduralTextures.cpp
    src/Textures/Effects/TextureEffectManager.cpp
    src/Textures/Effects/TextureUtils.cpp
    src/Textures/Effects/UVEffects.cpp
)

set(SHADER_SOURCES
    src/Shaders/ShaderManager.cpp
    src/Shaders/Effect.cpp
)

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${TEXTURE_SOURCES}
    ${SHADER_SOURCES}
    src/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${ALL_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src/
    sdk/
    ${DirectX9_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${DirectX9_LIBRARY}
    ${D3DX9_LIBRARY}
    d3dx9
    dxguid
    winmm
    comctl32
)

# Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    UNICODE
    _UNICODE
)

# Set Windows subsystem for WinMain
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

message(STATUS "DirectX 9 Include: ${DirectX9_INCLUDE_DIR}")
message(STATUS "DirectX 9 Library: ${DirectX9_LIBRARY}")
message(STATUS "D3DX9 Library: ${D3DX9_LIBRARY}")
