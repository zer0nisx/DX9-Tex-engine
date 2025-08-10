#include "AnimatedEffects.h"
#include "NoiseGenerator.h"
#include "../Texture.h"
#include <cmath>
#include <algorithm>

namespace TextureEffects {

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
            float animX = fx + params.scrollSpeedU * params.time;
            float animY = fy + params.scrollSpeedV * params.time;

            // Generar noise para lava con múltiples octavas
            float noise = NoiseGenerator::Turbulence2D(animX, animY, params.noiseScale, 4);

            // Añadir pulsación
            float pulse = sinf(params.time * params.pulseFrequency) * 0.1f + 0.9f;
            noise *= pulse;

            // Crear efecto de flujo vertical
            float flow = sinf(animY * 8.0f + params.time * 3.0f) * 0.1f;
            noise += flow;

            noise = std::max(0.0f, std::min(1.0f, noise));

            // Interpolar colores
            D3DCOLOR color = NoiseGenerator::NoiseToColor(noise, params.baseColor, params.hotColor);

            // Añadir emisión para las zonas más calientes
            if (noise > 0.7f)
            {
                color = ApplyGlow(color, params.glowIntensity * (noise - 0.7f) / 0.3f);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

void AnimatedEffects::UpdateWaterTexture(std::shared_ptr<Texture> texture, const WaterParams& params)
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

            // Calcular ondas de agua
            float wave1 = CalculateWaveHeight(fx, fy, params.time, params.waveSpeed, params.waveScale);
            float wave2 = CalculateWaveHeight(fx * 1.3f, fy * 0.7f, params.time * 1.2f, params.waveSpeed * 0.8f, params.waveScale * 1.5f);

            float combinedWaves = (wave1 + wave2) * 0.5f;

            // Efecto de profundidad
            float depth = NoiseGenerator::Perlin2D(fx, fy, 2.0f, 3) * 0.3f + 0.7f;

            // Combinar profundidad con ondas
            float waterLevel = depth + combinedWaves * 0.2f;
            waterLevel = std::max(0.0f, std::min(1.0f, waterLevel));

            // Color base del agua
            D3DCOLOR waterColor = NoiseGenerator::NoiseToColor(waterLevel, params.deepColor, params.shallowColor);

            // Añadir espuma en las crestas de las ondas
            if (combinedWaves > 0.8f)
            {
                float foamStrength = (combinedWaves - 0.8f) / 0.2f * params.foamAmount;
                waterColor = NoiseGenerator::NoiseToColor(foamStrength, waterColor, params.foamColor);
            }

            // Añadir cáusticos
            if (params.causticStrength > 0.0f)
            {
                float caustics = NoiseGenerator::VoronoiNoise2D(fx + params.time * 0.1f, fy + params.time * 0.15f, 8.0f);
                caustics = NoiseGenerator::ThresholdNoise(caustics, 0.2f, 0.1f) * params.causticStrength;

                if (caustics > 0.0f)
                {
                    waterColor = ApplyGlow(waterColor, caustics);
                }
            }

            pixels[y * pitch + x] = waterColor;
        }
    }

    texture->Unlock();
}

void AnimatedEffects::UpdateFireTexture(std::shared_ptr<Texture> texture, const FireParams& params)
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

            // Calcular forma de llama
            float flameShape = CalculateFlameShape(fx, 1.0f - fy, params.flameHeight, params.time);

            // Añadir turbulencia
            float turbulence = NoiseGenerator::Turbulence2D(
                fx + params.windStrength * sinf(params.time * 2.0f),
                fy - params.time * 0.5f,
                params.turbulence,
                4
            );

            // Combinar forma y turbulencia
            float fireIntensity = flameShape * turbulence * params.intensity;
            fireIntensity = std::max(0.0f, std::min(1.0f, fireIntensity));

            D3DCOLOR color;
            if (fireIntensity > 0.1f)
            {
                // Interpolar entre colores de fuego
                float t = (fireIntensity - 0.1f) / 0.9f;
                color = NoiseGenerator::NoiseToColor(t, params.outerColor, params.innerColor);

                // Añadir brillo en el centro
                if (fireIntensity > 0.7f)
                {
                    float glow = (fireIntensity - 0.7f) / 0.3f;
                    color = ApplyGlow(color, glow * 0.5f);
                }
            }
            else
            {
                // Área sin fuego o humo
                float smoke = fireIntensity * 10.0f; // Amplificar valores bajos para humo
                smoke = std::max(0.0f, std::min(1.0f, smoke));
                color = NoiseGenerator::NoiseToColor(smoke, D3DCOLOR_ARGB(0, 0, 0, 0), params.smokeColor);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

void AnimatedEffects::UpdatePlasmaTexture(std::shared_ptr<Texture> texture, const PlasmaParams& params)
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

            // Crear múltiples ondas sinusoidales
            float wave1 = sinf((fx * params.frequency1 + params.time * params.speed) * 2.0f * 3.14159f) * params.amplitude;
            float wave2 = cosf((fy * params.frequency2 + params.time * params.speed * 0.8f) * 2.0f * 3.14159f) * params.amplitude;
            float wave3 = sinf(((fx + fy) * params.frequency3 + params.time * params.speed * 1.2f) * 2.0f * 3.14159f) * params.amplitude;

            // Combinar ondas
            float plasma = (wave1 + wave2 + wave3) / 3.0f;
            plasma = plasma * 0.5f + 0.5f; // Normalizar a [0, 1]

            // Crear gradientes de color suaves
            float r = sinf(plasma * 3.14159f + params.time * params.speed) * 0.5f + 0.5f;
            float g = sinf(plasma * 3.14159f + params.time * params.speed + 2.0f) * 0.5f + 0.5f;
            float b = sinf(plasma * 3.14159f + params.time * params.speed + 4.0f) * 0.5f + 0.5f;

            // Mezclar con los colores base
            BYTE red = static_cast<BYTE>(r * 255);
            BYTE green = static_cast<BYTE>(g * 255);
            BYTE blue = static_cast<BYTE>(b * 255);

            pixels[y * pitch + x] = D3DCOLOR_ARGB(255, red, green, blue);
        }
    }

    texture->Unlock();
}

void AnimatedEffects::UpdateElectricTexture(std::shared_ptr<Texture> texture, const ElectricParams& params)
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

            // Crear rayos eléctricos usando noise de ridge
            float electric = NoiseGenerator::Ridge2D(fx + params.time * 0.1f, fy, params.boltFrequency, 2);

            // Añadir parpadeo
            float flicker = sinf(params.time * params.flickerSpeed) * 0.3f + 0.7f;
            electric *= flicker;

            // Crear efecto de resplandor
            float distance = electric;
            float glow = 1.0f - std::min(1.0f, distance / params.glowRadius);
            glow = glow * glow; // Suavizar

            electric = std::max(electric, glow * 0.3f);
            electric *= params.intensity;
            electric = std::max(0.0f, std::min(1.0f, electric));

            D3DCOLOR color;
            if (electric > 0.8f)
            {
                // Núcleo brillante del rayo
                color = params.boltColor;
                color = ApplyGlow(color, (electric - 0.8f) / 0.2f);
            }
            else if (electric > 0.2f)
            {
                // Resplandor
                float t = (electric - 0.2f) / 0.6f;
                color = NoiseGenerator::NoiseToColor(t, D3DCOLOR_ARGB(0, 0, 0, 0), params.glowColor);
            }
            else
            {
                // Fondo
                color = D3DCOLOR_ARGB(0, 0, 0, 0);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

// Helper functions
float AnimatedEffects::CalculateFlameShape(float x, float y, float height, float time)
{
    // Forma básica de llama (más ancha en la base, se estrecha hacia arriba)
    float baseWidth = 0.3f + sinf(time * 3.0f) * 0.1f; // Fluctuación de la base
    float topWidth = 0.05f;

    float currentWidth = baseWidth + (topWidth - baseWidth) * y;
    float distanceFromCenter = fabsf(x - 0.5f);

    if (distanceFromCenter > currentWidth)
        return 0.0f;

    // Intensidad basada en altura y distancia del centro
    float heightFactor = (1.0f - y) * height;
    float centerFactor = 1.0f - (distanceFromCenter / currentWidth);

    return heightFactor * centerFactor;
}

float AnimatedEffects::CalculateWaveHeight(float x, float y, float time, float speed, float scale)
{
    float wave1 = sinf((x * scale + time * speed) * 2.0f * 3.14159f);
    float wave2 = cosf((y * scale * 1.3f + time * speed * 0.7f) * 2.0f * 3.14159f);

    return (wave1 + wave2) * 0.5f;
}

D3DCOLOR AnimatedEffects::ApplyGlow(D3DCOLOR baseColor, float glowIntensity)
{
    glowIntensity = std::max(0.0f, std::min(2.0f, glowIntensity));

    BYTE r = (baseColor >> 16) & 0xFF;
    BYTE g = (baseColor >> 8) & 0xFF;
    BYTE b = baseColor & 0xFF;
    BYTE a = (baseColor >> 24) & 0xFF;

    r = static_cast<BYTE>(std::min(255, static_cast<int>(r * (1.0f + glowIntensity))));
    g = static_cast<BYTE>(std::min(255, static_cast<int>(g * (1.0f + glowIntensity))));
    b = static_cast<BYTE>(std::min(255, static_cast<int>(b * (1.0f + glowIntensity))));

    return D3DCOLOR_ARGB(a, r, g, b);
}

void AnimatedEffects::UpdateEnergyTexture(std::shared_ptr<Texture> texture, const EnergyParams& params)
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

    float centerX = 0.5f;
    float centerY = 0.5f;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float fx = static_cast<float>(x) / width;
            float fy = static_cast<float>(y) / height;

            // Distance from center
            float dx = fx - centerX;
            float dy = fy - centerY;
            float distance = sqrtf(dx * dx + dy * dy);

            // Generate concentric energy rings
            float ring = sinf(distance * params.ringCount * 3.14159f * 2.0f + params.time * params.pulseSpeed) * 0.5f + 0.5f;

            // Add distortion using noise
            float distortion = NoiseGenerator::Perlin2D(fx * 4.0f + params.time, fy * 4.0f + params.time, 2.0f, 3) * params.distortion;
            ring += distortion;

            // Apply intensity and distance falloff
            float energy = ring * params.intensity * (1.0f - distance);
            energy = std::max(0.0f, std::min(1.0f, energy));

            // Convert to color
            D3DCOLOR color;
            if (energy > 0.5f)
            {
                // Interpolate between edge and core color
                float t = (energy - 0.5f) / 0.5f;
                color = NoiseGenerator::NoiseToColor(t, params.edgeColor, params.coreColor);
            }
            else if (energy > 0.1f)
            {
                // Fade to edge color
                float t = (energy - 0.1f) / 0.4f;
                color = NoiseGenerator::NoiseToColor(t, D3DCOLOR_ARGB(0, 0, 0, 0), params.edgeColor);
            }
            else
            {
                // Background
                color = D3DCOLOR_ARGB(0, 0, 0, 0);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

void AnimatedEffects::UpdateSwirlTexture(std::shared_ptr<Texture> texture, const SwirlParams& params)
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

            // Distance from swirl center
            float dx = fx - params.centerX;
            float dy = fy - params.centerY;
            float distance = sqrtf(dx * dx + dy * dy);

            // Calculate angle
            float angle = atan2f(dy, dx);

            // Apply swirl effect
            float swirlAmount = params.swirlStrength * (1.0f - distance) * params.time;
            angle += swirlAmount;

            // Create rotating spiral pattern
            float rotation = params.time * params.rotationSpeed;
            float spiral = sinf(distance * 8.0f + angle + rotation) * 0.5f + 0.5f;

            // Apply distance falloff
            float intensity = spiral * (1.0f - distance);
            intensity = std::max(0.0f, std::min(1.0f, intensity));

            // Convert to color
            D3DCOLOR color;
            if (intensity > 0.3f)
            {
                float t = (intensity - 0.3f) / 0.7f;
                color = NoiseGenerator::NoiseToColor(t, params.outerColor, params.centerColor);
            }
            else
            {
                float t = intensity / 0.3f;
                color = NoiseGenerator::NoiseToColor(t, D3DCOLOR_ARGB(0, 0, 0, 0), params.outerColor);
            }

            pixels[y * pitch + x] = color;
        }
    }

    texture->Unlock();
}

} // namespace TextureEffects
