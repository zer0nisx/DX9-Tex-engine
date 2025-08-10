# Texture Effects Modules

Este directorio contiene los módulos de efectos de texturas organizados de manera modular para facilitar el mantenimiento y evitar que los archivos crezcan desmedidamente.

## Estructura de Archivos

### Generación de Ruido
- **NoiseGenerator.h/.cpp**: Funciones para generar diferentes tipos de ruido (Perlin, Simplex, Turbulencia, etc.)
  - Ruido fractal y multifractal
  - Ruido Voronoi
  - Ruido con distorsión
  - Utilidades de combinación y umbralización

### Texturas Procedurales
- **ProceduralTextures.h/.cpp**: Generadores de texturas procedurales
  - Patrones básicos (tablero de ajedrez, rayas, gradientes)
  - Texturas de ruido
  - Texturas de materiales (madera, mármol, metal, ladrillo)
  - Texturas orgánicas (piel, cuero, roca)
  - Efectos especiales (eléctrico, cáusticos)

### Efectos Animados
- **AnimatedEffects.h/.cpp**: Efectos de texturas animadas
  - Efecto de lava con parámetros configurables
  - Efecto de agua con ondas y espuma
  - Efecto de fuego con turbulencia
  - Efecto de plasma multicolor
  - Efecto eléctrico con destellos
  - Efecto de campo de energía
  - Efecto de remolino

### Transformaciones UV
- **UVEffects.h/.cpp**: Transformaciones de coordenadas UV
  - Transformaciones básicas (escala, rotación, traslación)
  - Transformaciones animadas (ondas, pulso, orbital)
  - Transformaciones complejas (remolino, ondulación, turbulencia)
  - Efectos de perspectiva y distorsión
  - Combinaciones predefinidas

### Post-Procesamiento
- **PostEffects.h/.cpp**: Efectos de post-procesamiento
  - Manipulación de color (brillo, contraste, saturación, tono)
  - Filtros (blur, sharpen, emboss, detección de bordes)
  - Ruido y artefactos
  - Efectos de distorsión
  - Efectos artísticos (pintura al óleo, pixelado, mosaico)
  - Efectos de iluminación (resplandor, lens flare, rayos de dios)

### Utilidades
- **TextureUtils.h/.cpp**: Funciones utilitarias
  - Manipulación de colores
  - Funciones matemáticas
  - Conversiones de espacios de color
  - Sampling de texturas
  - Análisis de histograma
  - Funciones de easing

### Administrador de Efectos
- **TextureEffectManager.h/.cpp**: Administrador centralizado de efectos
  - Registro y administración de efectos animados
  - Control de tiempo global y escalado
  - Configuraciones predefinidas (presets)
  - Optimización de rendimiento
  - Estadísticas de rendimiento

## Uso

### Inclusión Principal
```cpp
#include "TextureEffects.h"  // Incluye todos los módulos
```

### Uso Básico
```cpp
// Crear ruido Perlin
float noise = TextureEffects::NoiseGenerator::Perlin2D(x, y, 4.0f, 4);

// Crear textura procedural
auto checkerTexture = TextureEffects::ProceduralTextures::CreateCheckerboard(
    device, 256, 256, 16, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(0, 0, 0));

// Registrar efecto animado
TextureEffects::AnimatedEffects::LavaParams lavaParams;
g_effectManager.RegisterLavaEffect(texture, lavaParams);

// Aplicar post-procesamiento
TextureEffects::PostEffects::ApplyBlur(texture, 2.0f);
```

### Compatibilidad hacia Atrás
La estructura modular mantiene compatibilidad completa con el código existente. Todas las clases y funciones siguen disponibles bajo el namespace `TextureEffects`.

## Ventajas de la Estructura Modular

1. **Mantenimiento**: Cada módulo es independiente y fácil de mantener
2. **Compilación**: Compilación más rápida al editar solo un módulo
3. **Reutilización**: Los módulos pueden reutilizarse en otros proyectos
4. **Claridad**: Funcionalidad organizada por categorías lógicas
5. **Escalabilidad**: Fácil agregar nuevos efectos sin afectar otros módulos

## Directrices para Nuevos Efectos

- Agregar nuevos tipos de ruido en `NoiseGenerator`
- Agregar nuevas texturas procedurales en `ProceduralTextures`
- Agregar nuevos efectos animados en `AnimatedEffects`
- Agregar nuevas utilidades matemáticas en `TextureUtils`
- Mantener la documentación actualizada

## Rendimiento

El administrador de efectos incluye optimizaciones:
- Límite de efectos por frame
- Escalado de tiempo independiente por efecto
- Estadísticas de rendimiento
- Pausa/reanudación de efectos individuales
