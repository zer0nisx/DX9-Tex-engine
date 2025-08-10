#include "NoiseGenerator.h"
#include <cmath>
#include <algorithm>

namespace TextureEffects {

float NoiseGenerator::Perlin2D(float x, float y, float frequency, int octaves, float persistence)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        // Simplified Perlin noise using sin/cos
        float noise = sinf(x * freq) * cosf(y * freq);
        value += noise * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;
        freq *= 2.0f;
    }

    return (value / maxValue) * 0.5f + 0.5f; // Normalize to [0, 1]
}

float NoiseGenerator::Simplex2D(float x, float y, float frequency)
{
    // Simplified simplex noise approximation
    float noise = sinf(x * frequency * 6.28f) * cosf(y * frequency * 6.28f);
    return noise * 0.5f + 0.5f;
}

float NoiseGenerator::Ridge2D(float x, float y, float frequency, int octaves)
{
    float noise = Perlin2D(x, y, frequency, octaves, 0.5f);
    return 1.0f - fabsf(noise * 2.0f - 1.0f);
}

float NoiseGenerator::Turbulence2D(float x, float y, float frequency, int octaves)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++)
    {
        float noise = fabsf(sinf(x * freq) * cosf(y * freq));
        value += noise * amplitude;

        amplitude *= 0.5f;
        freq *= 2.0f;
    }

    return std::min(1.0f, value);
}

float NoiseGenerator::FractalNoise2D(float x, float y, float frequency, int octaves, float persistence, float lacunarity)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        float noise = sinf(x * freq * 2.0f * 3.14159f) * cosf(y * freq * 2.0f * 3.14159f);
        value += noise * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;
        freq *= lacunarity;
    }

    return (value / maxValue) * 0.5f + 0.5f;
}

float NoiseGenerator::RidgedMultifractal2D(float x, float y, float frequency, int octaves, float persistence)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++)
    {
        float noise = sinf(x * freq * 2.0f * 3.14159f) * cosf(y * freq * 2.0f * 3.14159f);
        noise = 1.0f - fabsf(noise);
        noise *= noise; // Square for sharper ridges

        value += noise * amplitude;
        amplitude *= persistence;
        freq *= 2.0f;
    }

    return std::min(1.0f, value);
}

float NoiseGenerator::BillowNoise2D(float x, float y, float frequency, int octaves, float persistence)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++)
    {
        float noise = sinf(x * freq * 2.0f * 3.14159f) * cosf(y * freq * 2.0f * 3.14159f);
        noise = fabsf(noise); // Absolute value for billowy effect

        value += noise * amplitude;
        amplitude *= persistence;
        freq *= 2.0f;
    }

    return std::min(1.0f, value);
}

float NoiseGenerator::WarpedNoise2D(float x, float y, float warpStrength, float frequency)
{
    // Create warp vectors
    float warpX = Perlin2D(x, y, frequency * 0.5f, 3) * warpStrength;
    float warpY = Perlin2D(x + 100.0f, y, frequency * 0.5f, 3) * warpStrength;

    // Sample noise at warped position
    return Perlin2D(x + warpX, y + warpY, frequency, 4);
}

float NoiseGenerator::VoronoiNoise2D(float x, float y, float frequency)
{
    // Simple Voronoi approximation using cellular noise
    x *= frequency;
    y *= frequency;

    int cellX = static_cast<int>(floorf(x));
    int cellY = static_cast<int>(floorf(y));

    float minDistance = 999999.0f;

    for (int offsetY = -1; offsetY <= 1; offsetY++)
    {
        for (int offsetX = -1; offsetX <= 1; offsetX++)
        {
            int neighborX = cellX + offsetX;
            int neighborY = cellY + offsetY;

            // Generate pseudo-random point in this cell
            float pointX = neighborX + sinf(neighborX * 23.1f + neighborY * 19.7f) * 0.5f + 0.5f;
            float pointY = neighborY + cosf(neighborX * 31.3f + neighborY * 17.5f) * 0.5f + 0.5f;

            float dx = x - pointX;
            float dy = y - pointY;
            float distance = sqrtf(dx * dx + dy * dy);

            minDistance = std::min(minDistance, distance);
        }
    }

    return std::min(1.0f, minDistance);
}

D3DCOLOR NoiseGenerator::NoiseToColor(float noise, const D3DCOLOR& color1, const D3DCOLOR& color2)
{
    noise = std::max(0.0f, std::min(1.0f, noise));

    BYTE r1 = (color1 >> 16) & 0xFF;
    BYTE g1 = (color1 >> 8) & 0xFF;
    BYTE b1 = color1 & 0xFF;
    BYTE a1 = (color1 >> 24) & 0xFF;

    BYTE r2 = (color2 >> 16) & 0xFF;
    BYTE g2 = (color2 >> 8) & 0xFF;
    BYTE b2 = color2 & 0xFF;
    BYTE a2 = (color2 >> 24) & 0xFF;

    BYTE r = static_cast<BYTE>(static_cast<float>(r1) + (static_cast<float>(r2) - static_cast<float>(r1)) * noise);
    BYTE g = static_cast<BYTE>(static_cast<float>(g1) + (static_cast<float>(g2) - static_cast<float>(g1)) * noise);
    BYTE b = static_cast<BYTE>(static_cast<float>(b1) + (static_cast<float>(b2) - static_cast<float>(b1)) * noise);
    BYTE a = static_cast<BYTE>(static_cast<float>(a1) + (static_cast<float>(a2) - static_cast<float>(a1)) * noise);

    return D3DCOLOR_ARGB(a, r, g, b);
}

D3DCOLOR NoiseGenerator::NoiseToGrayscale(float noise)
{
    BYTE intensity = static_cast<BYTE>(std::max(0.0f, std::min(1.0f, noise)) * 255);
    return D3DCOLOR_ARGB(255, intensity, intensity, intensity);
}

float NoiseGenerator::RemapNoise(float noise, float newMin, float newMax)
{
    return newMin + (newMax - newMin) * noise;
}

float NoiseGenerator::CombineNoise(float noise1, float noise2, float blend)
{
    blend = std::max(0.0f, std::min(1.0f, blend));
    return noise1 * (1.0f - blend) + noise2 * blend;
}

float NoiseGenerator::ThresholdNoise(float noise, float threshold, float softness)
{
    if (softness <= 0.0f)
    {
        return noise >= threshold ? 1.0f : 0.0f;
    }

    float halfSoft = softness * 0.5f;
    if (noise < threshold - halfSoft)
        return 0.0f;
    else if (noise > threshold + halfSoft)
        return 1.0f;
    else
    {
        float t = (noise - (threshold - halfSoft)) / softness;
        return t * t * (3.0f - 2.0f * t); // Smooth step
    }
}

} // namespace TextureEffects
