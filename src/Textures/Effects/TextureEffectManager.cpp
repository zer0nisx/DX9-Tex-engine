#include "TextureEffectManager.h"
#include "AnimatedEffects.h"
#include "../Texture.h"
#include <algorithm>
#include <chrono>

namespace TextureEffects {

EffectManager::EffectManager()
    : m_globalTime(0.0f)
    , m_timeScale(1.0f)
    , m_maxEffectsPerFrame(10)
    , m_updateFrequency(60.0f)
    , m_lastUpdateTime(0.0f)
    , m_averageUpdateTime(0.0f)
    , m_updatesThisFrame(0)
    , m_frameCounter(0)
{
}

EffectManager::~EffectManager()
{
    UnregisterAllEffects();
}

void EffectManager::Update(float deltaTime)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    m_globalTime += deltaTime * m_timeScale;
    m_updatesThisFrame = 0;
    m_frameCounter++;

    // Update effects with frame limit
    size_t effectsToUpdate = std::min(m_effects.size(), m_maxEffectsPerFrame);

    for (size_t i = 0; i < effectsToUpdate; i++)
    {
        if (i < m_effects.size())
        {
            UpdateEffect(m_effects[i], deltaTime);
        }
    }

    // Calculate performance metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    float updateTime = duration.count() / 1000.0f; // Convert to milliseconds

    // Moving average for update time
    float alpha = 0.1f; // Smoothing factor
    m_averageUpdateTime = m_averageUpdateTime * (1.0f - alpha) + updateTime * alpha;
}

void EffectManager::RegisterLavaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::LavaParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::LavaParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateLavaTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Lava");
}

void EffectManager::RegisterWaterEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::WaterParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::WaterParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateWaterTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Water");
}

void EffectManager::RegisterFireEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::FireParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::FireParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateFireTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Fire");
}

void EffectManager::RegisterPlasmaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::PlasmaParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::PlasmaParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdatePlasmaTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Plasma");
}

void EffectManager::RegisterElectricEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::ElectricParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::ElectricParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateElectricTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Electric");
}

void EffectManager::RegisterEnergyEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::EnergyParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::EnergyParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateEnergyTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Energy");
}

void EffectManager::RegisterSwirlEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::SwirlParams& params)
{
    auto updateFunc = [params](std::shared_ptr<Texture> tex, float time) mutable {
        AnimatedEffects::SwirlParams animParams = params;
        animParams.time = time;
        AnimatedEffects::UpdateSwirlTexture(tex, animParams);
    };

    RegisterCustomEffect(texture, updateFunc, "Swirl");
}

void EffectManager::RegisterCustomEffect(std::shared_ptr<Texture> texture,
                                       std::function<void(std::shared_ptr<Texture>, float)> updateFunc,
                                       const std::string& name)
{
    if (!texture || !updateFunc)
        return;

    // Remove existing effect for this texture
    UnregisterEffect(texture);

    EffectEntry entry;
    entry.texture = texture;
    entry.updateFunc = updateFunc;
    entry.name = name;
    entry.lastUpdateTime = m_globalTime;

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

void EffectManager::UnregisterAllEffects()
{
    m_effects.clear();
}

void EffectManager::PauseEffect(std::shared_ptr<Texture> texture)
{
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
        [texture](EffectEntry& entry) {
            return entry.texture == texture;
        });

    if (it != m_effects.end())
    {
        it->isPaused = true;
    }
}

void EffectManager::ResumeEffect(std::shared_ptr<Texture> texture)
{
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
        [texture](EffectEntry& entry) {
            return entry.texture == texture;
        });

    if (it != m_effects.end())
    {
        it->isPaused = false;
    }
}

void EffectManager::SetEffectTimeScale(std::shared_ptr<Texture> texture, float scale)
{
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
        [texture](EffectEntry& entry) {
            return entry.texture == texture;
        });

    if (it != m_effects.end())
    {
        it->timeScale = scale;
    }
}

bool EffectManager::HasEffect(std::shared_ptr<Texture> texture) const
{
    return std::find_if(m_effects.begin(), m_effects.end(),
        [texture](const EffectEntry& entry) {
            return entry.texture == texture;
        }) != m_effects.end();
}

std::vector<std::shared_ptr<Texture>> EffectManager::GetActiveTextures() const
{
    std::vector<std::shared_ptr<Texture>> textures;
    textures.reserve(m_effects.size());

    for (const auto& effect : m_effects)
    {
        textures.push_back(effect.texture);
    }

    return textures;
}

void EffectManager::UpdateEffect(EffectEntry& effect, float deltaTime)
{
    if (effect.isPaused || !effect.texture || !effect.updateFunc)
        return;

    if (!ShouldUpdateEffect(effect, m_globalTime))
        return;

    try
    {
        float effectTime = m_globalTime * effect.timeScale;
        effect.updateFunc(effect.texture, effectTime);
        effect.lastUpdateTime = m_globalTime;
        m_updatesThisFrame++;
    }
    catch (...)
    {
        // Remove problematic effect
        UnregisterEffect(effect.texture);
    }
}

bool EffectManager::ShouldUpdateEffect(const EffectEntry& effect, float currentTime) const
{
    if (effect.updateInterval <= 0.0f)
        return true; // Update every frame

    return (currentTime - effect.lastUpdateTime) >= effect.updateInterval;
}

// Preset configurations
namespace Presets {

AnimatedEffects::LavaParams CreateLavaPreset(const std::string& presetName)
{
    AnimatedEffects::LavaParams params;

    if (presetName == "fast")
    {
        params.scrollSpeedU = 0.2f;
        params.scrollSpeedV = 0.1f;
        params.pulseFrequency = 2.0f;
        params.glowIntensity = 3.0f;
    }
    else if (presetName == "slow")
    {
        params.scrollSpeedU = 0.05f;
        params.scrollSpeedV = 0.025f;
        params.pulseFrequency = 0.5f;
        params.glowIntensity = 1.5f;
    }
    else if (presetName == "intense")
    {
        params.baseColor = D3DCOLOR_XRGB(200, 50, 0);
        params.hotColor = D3DCOLOR_XRGB(255, 200, 50);
        params.noiseScale = 4.0f;
        params.glowIntensity = 4.0f;
        params.pulseFrequency = 3.0f;
    }
    // Default values are already set in struct

    return params;
}

AnimatedEffects::WaterParams CreateWaterPreset(const std::string& presetName)
{
    AnimatedEffects::WaterParams params;

    if (presetName == "ocean")
    {
        params.deepColor = D3DCOLOR_XRGB(0, 30, 100);
        params.shallowColor = D3DCOLOR_XRGB(0, 100, 200);
        params.waveSpeed = 0.5f;
        params.waveScale = 2.0f;
        params.foamAmount = 0.1f;
    }
    else if (presetName == "tropical")
    {
        params.deepColor = D3DCOLOR_XRGB(0, 50, 150);
        params.shallowColor = D3DCOLOR_XRGB(50, 200, 255);
        params.waveSpeed = 1.2f;
        params.waveScale = 6.0f;
        params.foamAmount = 0.4f;
        params.causticStrength = 0.8f;
    }
    else if (presetName == "stormy")
    {
        params.deepColor = D3DCOLOR_XRGB(20, 30, 40);
        params.shallowColor = D3DCOLOR_XRGB(60, 80, 100);
        params.waveSpeed = 2.0f;
        params.waveScale = 8.0f;
        params.foamAmount = 0.6f;
    }

    return params;
}

AnimatedEffects::FireParams CreateFirePreset(const std::string& presetName)
{
    AnimatedEffects::FireParams params;

    if (presetName == "hot")
    {
        params.innerColor = D3DCOLOR_XRGB(255, 255, 200);
        params.outerColor = D3DCOLOR_XRGB(255, 100, 0);
        params.intensity = 1.5f;
        params.turbulence = 3.0f;
    }
    else if (presetName == "cold")
    {
        params.innerColor = D3DCOLOR_XRGB(200, 200, 255);
        params.outerColor = D3DCOLOR_XRGB(100, 150, 255);
        params.intensity = 1.0f;
        params.turbulence = 1.5f;
    }
    else if (presetName == "magical")
    {
        params.innerColor = D3DCOLOR_XRGB(255, 200, 255);
        params.outerColor = D3DCOLOR_XRGB(150, 0, 255);
        params.intensity = 1.2f;
        params.turbulence = 2.5f;
        params.windStrength = 0.05f;
    }

    return params;
}

AnimatedEffects::PlasmaParams CreatePlasmaPreset(const std::string& presetName)
{
    AnimatedEffects::PlasmaParams params;

    if (presetName == "rainbow")
    {
        params.color1 = D3DCOLOR_XRGB(255, 0, 0);
        params.color2 = D3DCOLOR_XRGB(0, 255, 0);
        params.color3 = D3DCOLOR_XRGB(0, 0, 255);
        params.speed = 0.8f;
    }
    else if (presetName == "electric")
    {
        params.color1 = D3DCOLOR_XRGB(255, 255, 255);
        params.color2 = D3DCOLOR_XRGB(100, 100, 255);
        params.color3 = D3DCOLOR_XRGB(200, 200, 255);
        params.frequency1 = 4.0f;
        params.frequency2 = 6.0f;
        params.frequency3 = 8.0f;
        params.speed = 2.0f;
    }
    else if (presetName == "alien")
    {
        params.color1 = D3DCOLOR_XRGB(0, 255, 0);
        params.color2 = D3DCOLOR_XRGB(255, 0, 255);
        params.color3 = D3DCOLOR_XRGB(255, 255, 0);
        params.frequency1 = 1.5f;
        params.frequency2 = 2.5f;
        params.frequency3 = 3.5f;
        params.speed = 0.6f;
    }

    return params;
}

AnimatedEffects::ElectricParams CreateElectricPreset(const std::string& presetName)
{
    AnimatedEffects::ElectricParams params;

    if (presetName == "lightning")
    {
        params.boltColor = D3DCOLOR_XRGB(255, 255, 255);
        params.glowColor = D3DCOLOR_XRGB(150, 150, 255);
        params.boltFrequency = 12.0f;
        params.intensity = 2.0f;
        params.flickerSpeed = 15.0f;
    }
    else if (presetName == "energy")
    {
        params.boltColor = D3DCOLOR_XRGB(0, 255, 255);
        params.glowColor = D3DCOLOR_XRGB(0, 150, 200);
        params.boltFrequency = 6.0f;
        params.intensity = 1.5f;
        params.flickerSpeed = 8.0f;
        params.glowRadius = 0.15f;
    }
    else if (presetName == "magic")
    {
        params.boltColor = D3DCOLOR_XRGB(255, 100, 255);
        params.glowColor = D3DCOLOR_XRGB(200, 0, 200);
        params.boltFrequency = 4.0f;
        params.intensity = 1.2f;
        params.flickerSpeed = 5.0f;
        params.glowRadius = 0.2f;
    }

    return params;
}

} // namespace Presets

} // namespace TextureEffects
