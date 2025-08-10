#include "ProceduralTextures.h"
#include "NoiseGenerator.h"
#include "../Texture.h"
#include <cmath>
#include <algorithm>

namespace TextureEffects {

std::shared_ptr<Texture> ProceduralTextures::CreateCheckerboard(IDirect3DDevice9* device, int width, int height,
                                                               int checkerSize, D3DCOLOR color1, D3DCOLOR color2)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            bool checker = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            pixels[y * pitch + x] = checker ? color1 : color2;
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateStripes(IDirect3DDevice9* device, int width, int height,
                                                          int stripeWidth, D3DCOLOR color1, D3DCOLOR color2, bool vertical)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int coord = vertical ? x : y;
            bool stripe = (coord / stripeWidth) % 2 == 0;
            pixels[y * pitch + x] = stripe ? color1 : color2;
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateGradient(IDirect3DDevice9* device, int width, int height,
                                                           D3DCOLOR startColor, D3DCOLOR endColor, bool radial)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    float centerX = width * 0.5f;
    float centerY = height * 0.5f;
    float maxRadius = sqrtf(centerX * centerX + centerY * centerY);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float t;
            if (radial)
            {
                float dx = x - centerX;
                float dy = y - centerY;
                float distance = sqrtf(dx * dx + dy * dy);
                t = std::min(1.0f, distance / maxRadius);
            }
            else
            {
                t = static_cast<float>(x) / static_cast<float>(width - 1);
            }

            pixels[y * pitch + x] = NoiseGenerator::NoiseToColor(t, startColor, endColor);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreatePerlinNoise(IDirect3DDevice9* device, int width, int height,
                                                              float frequency, int octaves)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            float noise = NoiseGenerator::Perlin2D(fx, fy, frequency, octaves, 0.5f);
            pixels[y * pitch + x] = NoiseGenerator::NoiseToGrayscale(noise);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateTurbulence(IDirect3DDevice9* device, int width, int height,
                                                             float frequency, int octaves)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            float noise = NoiseGenerator::Turbulence2D(fx, fy, frequency, octaves);
            pixels[y * pitch + x] = NoiseGenerator::NoiseToGrayscale(noise);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateClouds(IDirect3DDevice9* device, int width, int height,
                                                         float frequency, int octaves)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    D3DCOLOR skyColor = D3DCOLOR_XRGB(135, 206, 250);   // Light blue
    D3DCOLOR cloudColor = D3DCOLOR_XRGB(255, 255, 255); // White

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            float noise = NoiseGenerator::FractalNoise2D(fx, fy, frequency, octaves, 0.6f, 2.0f);
            noise = NoiseGenerator::ThresholdNoise(noise, 0.4f, 0.2f);

            pixels[y * pitch + x] = NoiseGenerator::NoiseToColor(noise, skyColor, cloudColor);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateWoodGrain(IDirect3DDevice9* device, int width, int height,
                                                            D3DCOLOR lightWood, D3DCOLOR darkWood)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            // Create wood ring pattern
            float distance = sqrtf(fx * fx + fy * fy * 4.0f); // Elongate in Y
            float rings = sinf(distance * 20.0f) * 0.5f + 0.5f;

            // Add noise for natural variation
            float noise = NoiseGenerator::Perlin2D(fx, fy, 8.0f, 3, 0.3f);
            rings += noise * 0.3f;

            rings = std::max(0.0f, std::min(1.0f, rings));
            pixels[y * pitch + x] = NoiseGenerator::NoiseToColor(rings, darkWood, lightWood);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateMarble(IDirect3DDevice9* device, int width, int height,
                                                         D3DCOLOR baseColor, D3DCOLOR veinColor)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            // Create marble veins using warped noise
            float veins = NoiseGenerator::WarpedNoise2D(fx, fy, 0.1f, 4.0f);
            veins = NoiseGenerator::ThresholdNoise(veins, 0.6f, 0.1f);

            // Add subtle base noise
            float baseNoise = NoiseGenerator::Perlin2D(fx, fy, 2.0f, 2, 0.3f) * 0.2f;
            veins += baseNoise;

            veins = std::max(0.0f, std::min(1.0f, veins));
            pixels[y * pitch + x] = NoiseGenerator::NoiseToColor(veins, baseColor, veinColor);
        }
    }

    texture->Unlock();
    return texture;
}

std::shared_ptr<Texture> ProceduralTextures::CreateMetal(IDirect3DDevice9* device, int width, int height,
                                                        D3DCOLOR metalColor, float roughness)
{
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(device, width, height, D3DFMT_A8R8G8B8))
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return nullptr;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            // High frequency noise for metal surface
            float noise = NoiseGenerator::Perlin2D(fx, fy, 32.0f, 4, 0.3f);
            noise = noise * roughness + (1.0f - roughness) * 0.5f;

            // Add some scratches
            float scratches = NoiseGenerator::Perlin2D(fx, fy * 10.0f, 1.0f, 1) * 0.1f;
            noise += scratches;

            noise = std::max(0.0f, std::min(1.0f, noise));

            // Brighten/darken the metal color based on noise
            BYTE r = (metalColor >> 16) & 0xFF;
            BYTE g = (metalColor >> 8) & 0xFF;
            BYTE b = metalColor & 0xFF;

            float factor = 0.8f + noise * 0.4f; // Range [0.8, 1.2]
            r = static_cast<BYTE>(std::min(255, static_cast<int>(r * factor)));
            g = static_cast<BYTE>(std::min(255, static_cast<int>(g * factor)));
            b = static_cast<BYTE>(std::min(255, static_cast<int>(b * factor)));

            pixels[y * pitch + x] = D3DCOLOR_ARGB(255, r, g, b);
        }
    }

    texture->Unlock();
    return texture;
}

// Helper functions
float ProceduralTextures::CalculateDistance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

D3DCOLOR ProceduralTextures::BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend)
{
    return NoiseGenerator::NoiseToColor(blend, color1, color2);
}

void ProceduralTextures::FillSolidColor(std::shared_ptr<Texture> texture, D3DCOLOR color)
{
    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

} // namespace TextureEffects
