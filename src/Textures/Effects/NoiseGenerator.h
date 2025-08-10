#pragma once

#include <d3d9.h>

namespace TextureEffects {

    class NoiseGenerator {
    public:
        // Basic noise functions
        static float Perlin2D(float x, float y, float frequency = 1.0f, int octaves = 4, float persistence = 0.5f);
        static float Simplex2D(float x, float y, float frequency = 1.0f);
        static float Ridge2D(float x, float y, float frequency = 1.0f, int octaves = 4);
        static float Turbulence2D(float x, float y, float frequency = 1.0f, int octaves = 4);

        // Fractal noise variants
        static float FractalNoise2D(float x, float y, float frequency = 1.0f, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f);
        static float RidgedMultifractal2D(float x, float y, float frequency = 1.0f, int octaves = 4, float persistence = 0.5f);
        static float BillowNoise2D(float x, float y, float frequency = 1.0f, int octaves = 4, float persistence = 0.5f);

        // Warped noise
        static float WarpedNoise2D(float x, float y, float warpStrength = 0.1f, float frequency = 1.0f);
        static float VoronoiNoise2D(float x, float y, float frequency = 1.0f);

        // Utility functions
        static D3DCOLOR NoiseToColor(float noise, const D3DCOLOR& color1, const D3DCOLOR& color2);
        static D3DCOLOR NoiseToGrayscale(float noise);
        static float RemapNoise(float noise, float newMin = 0.0f, float newMax = 1.0f);

        // Advanced noise operations
        static float CombineNoise(float noise1, float noise2, float blend);
        static float ThresholdNoise(float noise, float threshold, float softness = 0.0f);
    };

}
