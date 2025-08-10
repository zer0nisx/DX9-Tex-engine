#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <memory>

class Texture;

namespace TextureEffects {

    class Utils {
    public:
        // Color manipulation utilities
        static D3DCOLOR InterpolateColor(D3DCOLOR color1, D3DCOLOR color2, float t);
        static D3DCOLOR BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend);
        static D3DCOLOR MultiplyColors(D3DCOLOR color1, D3DCOLOR color2);
        static D3DCOLOR AddColors(D3DCOLOR color1, D3DCOLOR color2);
        static D3DCOLOR SubtractColors(D3DCOLOR color1, D3DCOLOR color2);
        static D3DCOLOR ScreenColors(D3DCOLOR color1, D3DCOLOR color2);

        // Mathematical utilities
        static float Clamp(float value, float min, float max);
        static float Lerp(float a, float b, float t);
        static float SmoothStep(float edge0, float edge1, float x);
        static float SmootherStep(float edge0, float edge1, float x);
        static float Remap(float value, float oldMin, float oldMax, float newMin, float newMax);

        // Trigonometric helpers
        static float Sin01(float x); // Sine function that returns [0, 1]
        static float Cos01(float x); // Cosine function that returns [0, 1]
        static float Triangle(float x); // Triangle wave [0, 1]
        static float Sawtooth(float x); // Sawtooth wave [0, 1]
        static float Square(float x, float duty = 0.5f); // Square wave [0, 1]

        // Vector utilities
        static D3DXVECTOR2 PolarToCartesian(float radius, float angle);
        static void CartesianToPolar(const D3DXVECTOR2& pos, float& radius, float& angle);
        static float Distance2D(const D3DXVECTOR2& a, const D3DXVECTOR2& b);
        static D3DXVECTOR2 Normalize2D(const D3DXVECTOR2& v);
        static D3DXVECTOR2 Rotate2D(const D3DXVECTOR2& v, float angle);

        // Color space conversions
        static D3DXVECTOR3 RGBToHSV(D3DCOLOR color);
        static D3DCOLOR HSVToRGB(const D3DXVECTOR3& hsv);
        static D3DXVECTOR3 RGBToHSL(D3DCOLOR color);
        static D3DCOLOR HSLToRGB(const D3DXVECTOR3& hsl);
        static D3DXVECTOR3 RGBToLab(D3DCOLOR color);
        static D3DCOLOR LabToRGB(const D3DXVECTOR3& lab);

        // Luminance and gamma
        static float GetLuminance(D3DCOLOR color);
        static D3DCOLOR AdjustGamma(D3DCOLOR color, float gamma);
        static D3DCOLOR LinearToSRGB(D3DCOLOR color);
        static D3DCOLOR SRGBToLinear(D3DCOLOR color);

        // Texture utilities
        static bool CopyTexture(std::shared_ptr<Texture> source, std::shared_ptr<Texture> destination);
        static std::shared_ptr<Texture> CloneTexture(IDirect3DDevice9* device, std::shared_ptr<Texture> source);
        static std::shared_ptr<Texture> ResizeTexture(IDirect3DDevice9* device, std::shared_ptr<Texture> source, int newWidth, int newHeight);
        static std::shared_ptr<Texture> CropTexture(IDirect3DDevice9* device, std::shared_ptr<Texture> source,
                                                   int x, int y, int width, int height);

        // Sampling utilities
        static D3DCOLOR SampleNearest(std::shared_ptr<Texture> texture, float u, float v);
        static D3DCOLOR SampleBilinear(std::shared_ptr<Texture> texture, float u, float v);
        static D3DCOLOR SampleBicubic(std::shared_ptr<Texture> texture, float u, float v);

        // Histogram and analysis
        static void CalculateHistogram(std::shared_ptr<Texture> texture, int histogram[256], int channel = -1); // -1 for luminance
        static float CalculateAverageLuminance(std::shared_ptr<Texture> texture);
        static D3DCOLOR CalculateAverageColor(std::shared_ptr<Texture> texture);
        static void GetMinMaxLuminance(std::shared_ptr<Texture> texture, float& minLum, float& maxLum);

        // Validation and bounds checking
        static bool IsValidUV(float u, float v);
        static bool IsValidColor(D3DCOLOR color);
        static D3DXVECTOR2 WrapUV(const D3DXVECTOR2& uv);
        static D3DXVECTOR2 ClampUV(const D3DXVECTOR2& uv);

        // Random utilities
        static float RandomFloat(float min = 0.0f, float max = 1.0f);
        static int RandomInt(int min, int max);
        static D3DCOLOR RandomColor();
        static D3DXVECTOR2 RandomVector2D(float minX, float maxX, float minY, float maxY);

        // Easing functions
        static float EaseInQuad(float t);
        static float EaseOutQuad(float t);
        static float EaseInOutQuad(float t);
        static float EaseInCubic(float t);
        static float EaseOutCubic(float t);
        static float EaseInOutCubic(float t);
        static float EaseInSine(float t);
        static float EaseOutSine(float t);
        static float EaseInOutSine(float t);

    private:
        // Internal helper functions
        static float CubicInterpolate(float a, float b, float c, float d, float t);
        static D3DCOLOR GetPixelSafe(std::shared_ptr<Texture> texture, int x, int y);
    };

}
