#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <memory>
#include <functional>

class Texture;

namespace TextureEffects {

    // Noise generation
    class NoiseGenerator {
    public:
        static float Perlin2D(float x, float y, float frequency = 1.0f, int octaves = 4, float persistence = 0.5f);
        static float Simplex2D(float x, float y, float frequency = 1.0f);
        static float Ridge2D(float x, float y, float frequency = 1.0f, int octaves = 4);
        static float Turbulence2D(float x, float y, float frequency = 1.0f, int octaves = 4);
        static D3DCOLOR NoiseToColor(float noise, const D3DCOLOR& color1, const D3DCOLOR& color2);
    };

    // Procedural texture generators
    class ProceduralTextures {
    public:
        // Basic patterns
        static std::shared_ptr<Texture> CreateCheckerboard(IDirect3DDevice9* device, int width, int height,
                                                          int checkerSize, D3DCOLOR color1, D3DCOLOR color2);
        static std::shared_ptr<Texture> CreateStripes(IDirect3DDevice9* device, int width, int height,
                                                     int stripeWidth, D3DCOLOR color1, D3DCOLOR color2, bool vertical = false);
        static std::shared_ptr<Texture> CreateGradient(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR startColor, D3DCOLOR endColor, bool radial = false);

        // Noise textures
        static std::shared_ptr<Texture> CreatePerlinNoise(IDirect3DDevice9* device, int width, int height,
                                                         float frequency = 4.0f, int octaves = 4);
        static std::shared_ptr<Texture> CreateTurbulence(IDirect3DDevice9* device, int width, int height,
                                                        float frequency = 4.0f, int octaves = 6);
        static std::shared_ptr<Texture> CreateClouds(IDirect3DDevice9* device, int width, int height,
                                                    float frequency = 2.0f, int octaves = 5);

        // Material textures
        static std::shared_ptr<Texture> CreateWoodGrain(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR lightWood, D3DCOLOR darkWood);
        static std::shared_ptr<Texture> CreateMarble(IDirect3DDevice9* device, int width, int height,
                                                    D3DCOLOR baseColor, D3DCOLOR veinColor);
        static std::shared_ptr<Texture> CreateMetal(IDirect3DDevice9* device, int width, int height,
                                                   D3DCOLOR metalColor, float roughness = 0.2f);
    };

    // Animated effect generators
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
            float time = 0.0f;
        };

        static void UpdateLavaTexture(std::shared_ptr<Texture> texture, const LavaParams& params);

        // Water effect
        struct WaterParams {
            D3DCOLOR shallowColor = D3DCOLOR_XRGB(0, 150, 255);
            D3DCOLOR deepColor = D3DCOLOR_XRGB(0, 50, 150);
            float waveSpeed = 1.0f;
            float waveScale = 4.0f;
            float foamAmount = 0.3f;
            float time = 0.0f;
        };

        static void UpdateWaterTexture(std::shared_ptr<Texture> texture, const WaterParams& params);

        // Fire effect
        struct FireParams {
            D3DCOLOR innerColor = D3DCOLOR_XRGB(255, 255, 100);
            D3DCOLOR outerColor = D3DCOLOR_XRGB(255, 50, 0);
            float flameHeight = 1.0f;
            float intensity = 1.0f;
            float turbulence = 2.0f;
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
            float time = 0.0f;
        };

        static void UpdatePlasmaTexture(std::shared_ptr<Texture> texture, const PlasmaParams& params);
    };

    // UV transformation effects
    class UVEffects {
    public:
        // UV scrolling
        static D3DXMATRIX CreateScrollMatrix(float offsetU, float offsetV);
        static D3DXMATRIX CreateScaleMatrix(float scaleU, float scaleV);
        static D3DXMATRIX CreateRotationMatrix(float angle);
        static D3DXMATRIX CreateSinWaveMatrix(float time, float frequency, float amplitude);

        // Complex transformations
        static D3DXMATRIX CreateSwirlMatrix(float time, float intensity, const D3DXVECTOR2& center);
        static D3DXMATRIX CreateRippleMatrix(float time, float frequency, float amplitude, const D3DXVECTOR2& center);
        static D3DXMATRIX CreateTurbulenceMatrix(float time, float intensity);
    };

    // Post-processing effects
    class PostEffects {
    public:
        // Color manipulation
        static void AdjustBrightness(std::shared_ptr<Texture> texture, float brightness);
        static void AdjustContrast(std::shared_ptr<Texture> texture, float contrast);
        static void AdjustSaturation(std::shared_ptr<Texture> texture, float saturation);
        static void AdjustHue(std::shared_ptr<Texture> texture, float hueShift);

        // Filters
        static void ApplyBlur(std::shared_ptr<Texture> texture, float radius);
        static void ApplySharpen(std::shared_ptr<Texture> texture, float amount);
        static void ApplyEmboss(std::shared_ptr<Texture> texture, float strength);
        static void ApplyEdgeDetection(std::shared_ptr<Texture> texture);

        // Distortion
        static void ApplyDistortion(std::shared_ptr<Texture> texture, float amount, const D3DXVECTOR2& center);
        static void ApplyRipple(std::shared_ptr<Texture> texture, float amplitude, float frequency, const D3DXVECTOR2& center);
    };

    // Utility functions
    class Utils {
    public:
        static D3DCOLOR InterpolateColor(D3DCOLOR color1, D3DCOLOR color2, float t);
        static D3DCOLOR BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend);
        static float Clamp(float value, float min, float max);
        static float Lerp(float a, float b, float t);
        static float SmoothStep(float edge0, float edge1, float x);
        static D3DXVECTOR2 PolarToCartesian(float radius, float angle);
        static void CartesianToPolar(const D3DXVECTOR2& pos, float& radius, float& angle);

        // Color space conversions
        static D3DXVECTOR3 RGBToHSV(D3DCOLOR color);
        static D3DCOLOR HSVToRGB(const D3DXVECTOR3& hsv);
        static float GetLuminance(D3DCOLOR color);
    };

    // Effect manager for coordinating multiple effects
    class EffectManager {
    public:
        EffectManager();
        ~EffectManager();

        void Update(float deltaTime);
        void RegisterLavaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::LavaParams& params);
        void RegisterWaterEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::WaterParams& params);
        void RegisterFireEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::FireParams& params);
        void RegisterPlasmaEffect(std::shared_ptr<Texture> texture, const AnimatedEffects::PlasmaParams& params);

        void UnregisterEffect(std::shared_ptr<Texture> texture);
        void SetGlobalTimeScale(float scale) { m_timeScale = scale; }

    private:
        struct EffectEntry {
            std::shared_ptr<Texture> texture;
            std::function<void(float)> updateFunc;
        };

        std::vector<EffectEntry> m_effects;
        float m_globalTime;
        float m_timeScale;
    };
}

} // namespace TextureEffects

// Global effect manager instance
extern TextureEffects::EffectManager g_effectManager;
