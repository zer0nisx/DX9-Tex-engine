#include "TextureEffects.h"
#include "Texture.h"
#include <cmath>
#include <algorithm>
#include <windows.h>

// Global effect manager instance
namespace TextureEffects {
    EffectManager g_effectManager;
}

namespace TextureEffects {

// =================== NoiseGenerator ===================

float NoiseGenerator::Perlin2D(float x, float y, float frequency, int octaves, float persistence)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++)
    {
        // Simplified Perlin noise using sin/cos
        float noise = sinf(x * freq) * cosf(y * freq);
        value += noise * amplitude;

        amplitude *= persistence;
        freq *= 2.0f;
    }

    return value * 0.5f + 0.5f; // Normalize to [0, 1]
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

    return value;
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

// =================== ProceduralTextures ===================

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
            BYTE intensity = static_cast<BYTE>(noise * 255);

            pixels[y * pitch + x] = D3DCOLOR_ARGB(255, intensity, intensity, intensity);
        }
    }

    texture->Unlock();
    return texture;
}

// =================== AnimatedEffects ===================

void AnimatedEffects::UpdateLavaTexture(std::shared_ptr<Texture> texture, const LavaParams& params)
{
    if (!texture)
        return;

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
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            // Animación con offset temporal
            fx += params.scrollSpeedU * params.time;
            fy += params.scrollSpeedV * params.time;

            // Generar noise para lava
            float noise = NoiseGenerator::Turbulence2D(fx, fy, params.noiseScale, 4);
            noise += sinf(params.time * 2.0f) * 0.1f; // Pulsación

            // Interpolar colores
            D3DCOLOR color = NoiseGenerator::NoiseToColor(noise, params.baseColor, params.hotColor);

            // Añadir emisión
            if (noise > 0.7f)
            {
                BYTE r = static_cast<BYTE>(std::min(255, static_cast<int>((color >> 16) & 0xFF) +
                                 static_cast<int>(static_cast<float>((params.hotColor >> 16) & 0xFF) * params.glowIntensity * 0.3f)));
                BYTE g = static_cast<BYTE>(std::min(255, static_cast<int>((color >> 8) & 0xFF) +
                                 static_cast<int>(static_cast<float>((params.hotColor >> 8) & 0xFF) * params.glowIntensity * 0.3f)));
                BYTE b = (color & 0xFF);
                color = D3DCOLOR_ARGB(255, r, g, b);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

// =================== Utils ===================

D3DCOLOR Utils::InterpolateColor(D3DCOLOR color1, D3DCOLOR color2, float t)
{
    t = Clamp(t, 0.0f, 1.0f);

    BYTE r1 = (color1 >> 16) & 0xFF;
    BYTE g1 = (color1 >> 8) & 0xFF;
    BYTE b1 = color1 & 0xFF;
    BYTE a1 = (color1 >> 24) & 0xFF;

    BYTE r2 = (color2 >> 16) & 0xFF;
    BYTE g2 = (color2 >> 8) & 0xFF;
    BYTE b2 = color2 & 0xFF;
    BYTE a2 = (color2 >> 24) & 0xFF;

    BYTE r = static_cast<BYTE>(Lerp(static_cast<float>(r1), static_cast<float>(r2), t));
    BYTE g = static_cast<BYTE>(Lerp(static_cast<float>(g1), static_cast<float>(g2), t));
    BYTE b = static_cast<BYTE>(Lerp(static_cast<float>(b1), static_cast<float>(b2), t));
    BYTE a = static_cast<BYTE>(Lerp(static_cast<float>(a1), static_cast<float>(a2), t));

    return D3DCOLOR_ARGB(a, r, g, b);
}

float Utils::Clamp(float value, float min, float max)
{
    return std::max(min, std::min(max, value));
}

float Utils::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float Utils::SmoothStep(float edge0, float edge1, float x)
{
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// =================== EffectManager ===================

EffectManager::EffectManager()
    : m_globalTime(0.0f)
    , m_timeScale(1.0f)
{
}

EffectManager::~EffectManager() = default;

void EffectManager::Update(float deltaTime)
{
    m_globalTime += deltaTime * m_timeScale;

    // Actualizar todos los efectos registrados
    for (auto& effect : m_effects)
    {
        if (effect.updateFunc)
        {
            effect.updateFunc(m_globalTime);
        }
    }
}

void EffectManager::RegisterLavaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::LavaParams& params)
{
    EffectEntry entry;
    entry.texture = texture;
    entry.updateFunc = [texture, params](float time) mutable {
        AnimatedEffects::LavaParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateLavaTexture(texture, animParams);
    };

    m_effects.push_back(entry);
}

void EffectManager::UnregisterEffect(std::shared_ptr<Texture> texture)
{
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(),
            [texture](const EffectEntry& entry) {
                return entry.texture == texture;
            }),
        m_effects.end()
    );
}

} // namespace TextureEffects
