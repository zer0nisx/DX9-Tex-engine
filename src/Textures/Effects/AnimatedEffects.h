#pragma once

#include <d3d9.h>
#include <memory>

class Texture;

namespace TextureEffects {

    class AnimatedEffects {
    public:
        // Lava effect
        struct LavaParams {
            D3DCOLOR baseColor = D3DCOLOR_XRGB(255, 100, 0);
            D3DCOLOR hotColor = D3DCOLOR_XRGB(255, 255, 100);
            float scrollSpeedU = 0.1f;
            float scrollSpeedV = 0.05f;
            float noiseScale = 2.0f;
            float glowIntensity = 2.0f;
            float pulseFrequency = 1.0f;
            float time = 0.0f;
        };

        static void UpdateLavaTexture(std::shared_ptr<Texture> texture, const LavaParams& params);

        // Water effect
        struct WaterParams {
            D3DCOLOR shallowColor = D3DCOLOR_XRGB(0, 150, 255);
            D3DCOLOR deepColor = D3DCOLOR_XRGB(0, 50, 150);
            D3DCOLOR foamColor = D3DCOLOR_XRGB(255, 255, 255);
            float waveSpeed = 1.0f;
            float waveScale = 4.0f;
            float foamAmount = 0.3f;
            float causticStrength = 0.5f;
            float time = 0.0f;
        };

        static void UpdateWaterTexture(std::shared_ptr<Texture> texture, const WaterParams& params);

        // Fire effect
        struct FireParams {
            D3DCOLOR innerColor = D3DCOLOR_XRGB(255, 255, 100);
            D3DCOLOR outerColor = D3DCOLOR_XRGB(255, 50, 0);
            D3DCOLOR smokeColor = D3DCOLOR_XRGB(64, 64, 64);
            float flameHeight = 1.0f;
            float intensity = 1.0f;
            float turbulence = 2.0f;
            float windStrength = 0.1f;
            float time = 0.0f;
        };

        static void UpdateFireTexture(std::shared_ptr<Texture> texture, const FireParams& params);

        // Plasma effect
        struct PlasmaParams {
            D3DCOLOR color1 = D3DCOLOR_XRGB(255, 0, 255);
            D3DCOLOR color2 = D3DCOLOR_XRGB(0, 255, 255);
            D3DCOLOR color3 = D3DCOLOR_XRGB(255, 255, 0);
            float frequency1 = 2.0f;
            float frequency2 = 3.0f;
            float frequency3 = 4.0f;
            float speed = 1.0f;
            float amplitude = 1.0f;
            float time = 0.0f;
        };

        static void UpdatePlasmaTexture(std::shared_ptr<Texture> texture, const PlasmaParams& params);

        // Electric effect
        struct ElectricParams {
            D3DCOLOR boltColor = D3DCOLOR_XRGB(200, 200, 255);
            D3DCOLOR glowColor = D3DCOLOR_XRGB(100, 100, 255);
            float boltFrequency = 8.0f;
            float glowRadius = 0.1f;
            float intensity = 1.0f;
            float flickerSpeed = 10.0f;
            float time = 0.0f;
        };

        static void UpdateElectricTexture(std::shared_ptr<Texture> texture, const ElectricParams& params);

        // Energy field effect
        struct EnergyParams {
            D3DCOLOR coreColor = D3DCOLOR_XRGB(255, 255, 255);
            D3DCOLOR edgeColor = D3DCOLOR_XRGB(0, 255, 255);
            float pulseSpeed = 2.0f;
            float ringCount = 5.0f;
            float distortion = 0.2f;
            float intensity = 1.0f;
            float time = 0.0f;
        };

        static void UpdateEnergyTexture(std::shared_ptr<Texture> texture, const EnergyParams& params);

        // Swirl effect
        struct SwirlParams {
            D3DCOLOR centerColor = D3DCOLOR_XRGB(255, 255, 255);
            D3DCOLOR outerColor = D3DCOLOR_XRGB(0, 0, 0);
            float rotationSpeed = 1.0f;
            float swirlStrength = 2.0f;
            float centerX = 0.5f;
            float centerY = 0.5f;
            float time = 0.0f;
        };

        static void UpdateSwirlTexture(std::shared_ptr<Texture> texture, const SwirlParams& params);

    private:
        // Helper functions
        static float CalculateFlameShape(float x, float y, float height, float time);
        static float CalculateWaveHeight(float x, float y, float time, float speed, float scale);
        static D3DCOLOR ApplyGlow(D3DCOLOR baseColor, float glowIntensity);
    };

}
