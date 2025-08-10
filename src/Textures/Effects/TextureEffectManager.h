#pragma once

#include <d3d9.h>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <string>
#include "AnimatedEffects.h"

class Texture;

namespace TextureEffects {

    // Effect manager for coordinating multiple effects
    class EffectManager {
    public:
        EffectManager();
        ~EffectManager();

        // Core functionality
        void Update(float deltaTime);
        void SetGlobalTimeScale(float scale) { m_timeScale = scale; }
        float GetGlobalTime() const { return m_globalTime; }

        // Animated effect registration
        void RegisterLavaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::LavaParams& params);
        void RegisterWaterEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::WaterParams& params);
        void RegisterFireEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::FireParams& params);
        void RegisterPlasmaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::PlasmaParams& params);
        void RegisterElectricEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::ElectricParams& params);
        void RegisterEnergyEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::EnergyParams& params);
        void RegisterSwirlEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::SwirlParams& params);

        // Generic effect registration
        void RegisterCustomEffect(std::shared_ptr<Texture> texture,
                                 std::function<void(std::shared_ptr<Texture>, float)> updateFunc,
                                 const std::string& name = "");

        // Effect management
        void UnregisterEffect(std::shared_ptr<Texture> texture);
        void UnregisterAllEffects();
        void PauseEffect(std::shared_ptr<Texture> texture);
        void ResumeEffect(std::shared_ptr<Texture> texture);
        void SetEffectTimeScale(std::shared_ptr<Texture> texture, float scale);

        // Effect querying
        bool HasEffect(std::shared_ptr<Texture> texture) const;
        size_t GetEffectCount() const { return m_effects.size(); }
        std::vector<std::shared_ptr<Texture>> GetActiveTextures() const;

        // Performance settings
        void SetMaxEffectsPerFrame(size_t maxEffects) { m_maxEffectsPerFrame = maxEffects; }
        void SetUpdateFrequency(float frequency) { m_updateFrequency = frequency; }

        // Statistics
        float GetAverageUpdateTime() const { return m_averageUpdateTime; }
        size_t GetUpdatesThisFrame() const { return m_updatesThisFrame; }

    private:
        struct EffectEntry {
            std::shared_ptr<Texture> texture;
            std::function<void(std::shared_ptr<Texture>, float)> updateFunc;
            std::string name;
            bool isPaused = false;
            float timeScale = 1.0f;
            float lastUpdateTime = 0.0f;
            float updateInterval = 0.0f; // 0 = every frame
        };

        std::vector<EffectEntry> m_effects;
        float m_globalTime;
        float m_timeScale;
        size_t m_maxEffectsPerFrame;
        float m_updateFrequency;

        // Performance tracking
        float m_lastUpdateTime;
        float m_averageUpdateTime;
        size_t m_updatesThisFrame;
        size_t m_frameCounter;

        // Internal helpers
        void UpdateEffect(EffectEntry& effect, float deltaTime);
        bool ShouldUpdateEffect(const EffectEntry& effect, float currentTime) const;
    };

    // Preset effect configurations
    namespace Presets {
        AnimatedEffects::LavaParams CreateLavaPreset(const std::string& presetName = "default");
        AnimatedEffects::WaterParams CreateWaterPreset(const std::string& presetName = "default");
        AnimatedEffects::FireParams CreateFirePreset(const std::string& presetName = "default");
        AnimatedEffects::PlasmaParams CreatePlasmaPreset(const std::string& presetName = "default");
        AnimatedEffects::ElectricParams CreateElectricPreset(const std::string& presetName = "default");
    }

}
