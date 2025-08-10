#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <memory>

class Texture;

namespace TextureEffects {

    class PostEffects {
    public:
        // Color manipulation
        static void AdjustBrightness(std::shared_ptr<Texture> texture, float brightness);
        static void AdjustContrast(std::shared_ptr<Texture> texture, float contrast);
        static void AdjustSaturation(std::shared_ptr<Texture> texture, float saturation);
        static void AdjustHue(std::shared_ptr<Texture> texture, float hueShift);
        static void AdjustGamma(std::shared_ptr<Texture> texture, float gamma);

        // Advanced color operations
        static void ColorBalance(std::shared_ptr<Texture> texture, float redBalance, float greenBalance, float blueBalance);
        static void Posterize(std::shared_ptr<Texture> texture, int levels);
        static void Threshold(std::shared_ptr<Texture> texture, float threshold, bool binary = true);
        static void Invert(std::shared_ptr<Texture> texture);
        static void Sepia(std::shared_ptr<Texture> texture, float intensity = 1.0f);

        // Filters
        static void ApplyBlur(std::shared_ptr<Texture> texture, float radius);
        static void ApplyGaussianBlur(std::shared_ptr<Texture> texture, float sigma);
        static void ApplyMotionBlur(std::shared_ptr<Texture> texture, float angle, float distance);
        static void ApplySharpen(std::shared_ptr<Texture> texture, float amount);
        static void ApplyUnsharpMask(std::shared_ptr<Texture> texture, float amount, float radius);

        // Edge detection and enhancement
        static void ApplyEmboss(std::shared_ptr<Texture> texture, float strength, float angle = 45.0f);
        static void ApplyEdgeDetection(std::shared_ptr<Texture> texture, float threshold = 0.1f);
        static void ApplySobel(std::shared_ptr<Texture> texture);
        static void ApplyLaplacian(std::shared_ptr<Texture> texture);

        // Noise and artifacts
        static void AddNoise(std::shared_ptr<Texture> texture, float amount, bool monochrome = false);
        static void RemoveNoise(std::shared_ptr<Texture> texture, float strength);
        static void AddFilmGrain(std::shared_ptr<Texture> texture, float intensity, float size = 1.0f);
        static void AddVignette(std::shared_ptr<Texture> texture, float strength, float radius);

        // Distortion effects
        static void ApplyDistortion(std::shared_ptr<Texture> texture, float amount, const D3DXVECTOR2& center);
        static void ApplyRipple(std::shared_ptr<Texture> texture, float amplitude, float frequency, const D3DXVECTOR2& center);
        static void ApplySwirl(std::shared_ptr<Texture> texture, float angle, const D3DXVECTOR2& center, float radius);
        static void ApplyPinch(std::shared_ptr<Texture> texture, float amount, const D3DXVECTOR2& center, float radius);

        // Artistic effects
        static void OilPainting(std::shared_ptr<Texture> texture, int radius, int intensity);
        static void Pixelate(std::shared_ptr<Texture> texture, int pixelSize);
        static void Mosaic(std::shared_ptr<Texture> texture, int tileSize);
        static void CrossHatch(std::shared_ptr<Texture> texture, float strength, float spacing);

        // Lighting effects
        static void AddGlow(std::shared_ptr<Texture> texture, float intensity, float radius, D3DCOLOR glowColor);
        static void AddLensFlare(std::shared_ptr<Texture> texture, const D3DXVECTOR2& center, float intensity);
        static void AddGodRays(std::shared_ptr<Texture> texture, const D3DXVECTOR2& source, float intensity, int numRays);

    private:
        // Helper functions
        static void ApplyKernel(std::shared_ptr<Texture> texture, const float* kernel, int kernelSize, float divisor = 1.0f);
        static D3DCOLOR ApplyColorMatrix(D3DCOLOR color, const float matrix[4][4]);
        static D3DCOLOR BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend);
        static float CalculateDistance(float x1, float y1, float x2, float y2);
        static D3DCOLOR SampleBilinear(std::shared_ptr<Texture> texture, float u, float v);

        // Color space conversions
        static D3DXVECTOR3 RGBToHSV(D3DCOLOR color);
        static D3DCOLOR HSVToRGB(const D3DXVECTOR3& hsv);
        static float GetLuminance(D3DCOLOR color);
    };

}
