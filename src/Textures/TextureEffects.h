#pragma once

// Main header that includes all texture effect modules
// This provides a unified interface to all texture effects functionality

#include "Effects/NoiseGenerator.h"
#include "Effects/ProceduralTextures.h"
#include "Effects/AnimatedEffects.h"
#include "Effects/UVEffects.h"
#include "Effects/PostEffects.h"
#include "Effects/TextureUtils.h"
#include "Effects/TextureEffectManager.h"

// Re-export commonly used types and functions for backward compatibility
namespace TextureEffects {
    // Alias for Utils class to maintain backward compatibility
    using Utils = TextureEffects::Utils;
    using EffectManager = TextureEffects::EffectManager;
}

// Global effect manager instance
extern TextureEffects::EffectManager g_effectManager;

// Convenience macros for common operations
#define TEXTURE_NOISE(x, y, freq, oct) TextureEffects::NoiseGenerator::Perlin2D(x, y, freq, oct)
#define TEXTURE_BLEND(c1, c2, t) TextureEffects::Utils::InterpolateColor(c1, c2, t)
#define TEXTURE_CLAMP(val, min, max) TextureEffects::Utils::Clamp(val, min, max)
#define TEXTURE_LERP(a, b, t) TextureEffects::Utils::Lerp(a, b, t)
