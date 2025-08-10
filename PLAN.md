# Motor DirectX 9 - Sistema de Texturas Avanzadas

## 📋 Arquitectura del Motor

### 🏗️ Estructura del Proyecto
```
DX9Engine/
├── CMakeLists.txt
├── src/
│   ├── Core/
│   │   ├── Engine.cpp/h          # Núcleo principal
│   │   ├── Window.cpp/h          # Gestión de ventana
│   │   └── Timer.cpp/h           # Sistema de tiempo
│   ├── Graphics/
│   │   ├── Renderer.cpp/h        # Renderer principal DX9
│   │   ├── Mesh.cpp/h            # Gestión de mallas
│   │   └── Camera.cpp/h          # Sistema de cámara
│   ├── Textures/
│   │   ├── TextureManager.cpp/h  # Gestor de texturas
│   │   ├── Texture.cpp/h         # Clase textura individual
│   │   ├── Material.cpp/h        # Sistema de materiales
│   │   └── TextureEffects.cpp/h  # Efectos procedurales
│   ├── Shaders/
│   │   ├── ShaderManager.cpp/h   # Gestor de shaders
│   │   └── Effect.cpp/h          # Wrapper para efectos HLSL
│   └── main.cpp
├── shaders/
│   ├── basic.hlsl.txt           # Shader básico
│   ├── multitexture.hlsl.txt    # Multi-texturing
│   ├── lava_effect.hlsl.txt     # Efecto lava animado
│   ├── metal_reflection.hlsl.txt # Metal con reflexiones
│   ├── water_distortion.hlsl.txt # Agua con distorsión
│   └── procedural_noise.hlsl.txt # Ruido procedural
└── assets/
    └── textures/               # Texturas de ejemplo
```

## 🎯 Características Principales

### 1. Sistema de Texturas Multicapa
- **Hasta 8 texturas simultáneas** por material
- **Tipos soportados:** Diffuse, Normal, Specular, Metallic, Roughness, AO, Emission, Displacement
- **Formatos:** DDS, TGA, BMP, JPG, PNG
- **Blending modes:** Multiply, Add, Subtract, Overlay, Screen
- **UV Channels múltiples** para cada textura

### 2. Shaders Separados por Material
- **Vertex + Pixel Shaders** independientes
- **Parámetros dinámicos** modificables en runtime
- **Hot-reload** de shaders para desarrollo
- **Múltiples técnicas** por shader (calidad alta/media/baja)

### 3. Efectos Procedurales Animados
- **Lava:** UV scrolling + noise + emisión térmica
- **Metal:** Reflexiones cúbicas + fresnel + roughness variable
- **Agua:** Distorsión por normales + reflexión + refracción
- **Ruido:** Perlin/Simplex noise en tiempo real
- **Pulsaciones:** Efectos de respiración/latido

### 4. Sistema de Materiales Avanzado
```cpp
// Ejemplo de material multicapa
Material lava_material;
lava_material.AddTexture(DIFFUSE, "lava_base.dds");
lava_material.AddTexture(NORMAL, "lava_normal.dds");
lava_material.AddTexture(EMISSION, "lava_glow.dds");
lava_material.SetShader("lava_effect.hlsl");
lava_material.SetParameter("scroll_speed", 0.5f);
lava_material.SetParameter("glow_intensity", 2.0f);
```

## 🔧 Implementación Técnica

### DirectX 9 Features Utilizadas
- **Multi-texturing:** D3DTSS_COLOROP, D3DTSS_ALPHAOP
- **Vertex/Pixel Shaders 3.0:** Para efectos avanzados
- **Texture Stages:** Hasta 8 stages simultáneos
- **Alpha Blending:** Para transparencias y efectos
- **Cube Mapping:** Para reflexiones ambientales
- **Render to Texture:** Para efectos post-proceso

### Optimizaciones
- **Texture Pooling:** Reutilización de texturas
- **Batch Rendering:** Agrupación por material
- **LOD System:** Texturas de menor resolución a distancia
- **Compression:** DXT1/3/5 para texturas comprimidas
- **Mipmapping:** Generación automática de mipmaps

### Efectos Especiales
1. **UV Animation:** Scrolling, rotating, scaling
2. **Vertex Displacement:** Usando height maps
3. **Parallax Mapping:** Profundidad simulada
4. **Environment Mapping:** Reflexiones en tiempo real
5. **Multi-pass Rendering:** Para efectos complejos

## 📊 Casos de Uso

### Materiales Básicos
- **Madera:** Diffuse + Normal + Roughness
- **Metal:** Diffuse + Normal + Metallic + Reflection
- **Plástico:** Diffuse + Specular + Gloss

### Materiales Animados
- **Lava:** Base + Normal + Emission (animada)
- **Agua:** Refraction + Reflection + Normal (animada)
- **Cristal:** Transmission + Reflection + Caustics

### Efectos Ambientales
- **Niebla volumétrica:** Usando alpha blending
- **Partículas:** Texturas billboard animadas
- **Iluminación dinámica:** Shadow mapping básico

## 🚀 Roadmap de Desarrollo

### Fase 1: Core Engine
- Inicialización DirectX 9
- Window management
- Basic cube rendering

### Fase 2: Texture System
- TextureManager con pooling
- Multi-texture support
- Basic material system

### Fase 3: Shader System
- HLSL shader loading
- Parameter management
- Effect compilation

### Fase 4: Advanced Effects
- Procedural animations
- Multi-layer blending
- Environmental effects

### Fase 5: Optimization
- Performance profiling
- Memory optimization
- Batch rendering
