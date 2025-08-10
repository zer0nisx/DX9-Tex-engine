#include "TextureUtils.h"
#include "../Texture.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace TextureEffects {

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

D3DCOLOR Utils::BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend)
{
    return InterpolateColor(color1, color2, blend);
}

D3DCOLOR Utils::MultiplyColors(D3DCOLOR color1, D3DCOLOR color2)
{
    BYTE r1 = (color1 >> 16) & 0xFF;
    BYTE g1 = (color1 >> 8) & 0xFF;
    BYTE b1 = color1 & 0xFF;
    BYTE a1 = (color1 >> 24) & 0xFF;

    BYTE r2 = (color2 >> 16) & 0xFF;
    BYTE g2 = (color2 >> 8) & 0xFF;
    BYTE b2 = color2 & 0xFF;
    BYTE a2 = (color2 >> 24) & 0xFF;

    BYTE r = static_cast<BYTE>((r1 * r2) / 255);
    BYTE g = static_cast<BYTE>((g1 * g2) / 255);
    BYTE b = static_cast<BYTE>((b1 * b2) / 255);
    BYTE a = static_cast<BYTE>((a1 * a2) / 255);

    return D3DCOLOR_ARGB(a, r, g, b);
}

D3DCOLOR Utils::AddColors(D3DCOLOR color1, D3DCOLOR color2)
{
    BYTE r1 = (color1 >> 16) & 0xFF;
    BYTE g1 = (color1 >> 8) & 0xFF;
    BYTE b1 = color1 & 0xFF;
    BYTE a1 = (color1 >> 24) & 0xFF;

    BYTE r2 = (color2 >> 16) & 0xFF;
    BYTE g2 = (color2 >> 8) & 0xFF;
    BYTE b2 = color2 & 0xFF;
    BYTE a2 = (color2 >> 24) & 0xFF;

    BYTE r = static_cast<BYTE>(std::min(255, r1 + r2));
    BYTE g = static_cast<BYTE>(std::min(255, g1 + g2));
    BYTE b = static_cast<BYTE>(std::min(255, b1 + b2));
    BYTE a = static_cast<BYTE>(std::min(255, a1 + a2));

    return D3DCOLOR_ARGB(a, r, g, b);
}

D3DCOLOR Utils::ScreenColors(D3DCOLOR color1, D3DCOLOR color2)
{
    BYTE r1 = (color1 >> 16) & 0xFF;
    BYTE g1 = (color1 >> 8) & 0xFF;
    BYTE b1 = color1 & 0xFF;
    BYTE a1 = (color1 >> 24) & 0xFF;

    BYTE r2 = (color2 >> 16) & 0xFF;
    BYTE g2 = (color2 >> 8) & 0xFF;
    BYTE b2 = color2 & 0xFF;
    BYTE a2 = (color2 >> 24) & 0xFF;

    BYTE r = static_cast<BYTE>(255 - ((255 - r1) * (255 - r2)) / 255);
    BYTE g = static_cast<BYTE>(255 - ((255 - g1) * (255 - g2)) / 255);
    BYTE b = static_cast<BYTE>(255 - ((255 - b1) * (255 - b2)) / 255);
    BYTE a = static_cast<BYTE>(255 - ((255 - a1) * (255 - a2)) / 255);

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

float Utils::SmootherStep(float edge0, float edge1, float x)
{
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Utils::Remap(float value, float oldMin, float oldMax, float newMin, float newMax)
{
    float t = (value - oldMin) / (oldMax - oldMin);
    return newMin + t * (newMax - newMin);
}

float Utils::Sin01(float x)
{
    return sinf(x) * 0.5f + 0.5f;
}

float Utils::Cos01(float x)
{
    return cosf(x) * 0.5f + 0.5f;
}

float Utils::Triangle(float x)
{
    return 2.0f * fabsf(fmodf(x, 2.0f) - 1.0f);
}

float Utils::Sawtooth(float x)
{
    return fmodf(x, 1.0f);
}

float Utils::Square(float x, float duty)
{
    return fmodf(x, 1.0f) < duty ? 1.0f : 0.0f;
}

D3DXVECTOR2 Utils::PolarToCartesian(float radius, float angle)
{
    return D3DXVECTOR2(radius * cosf(angle), radius * sinf(angle));
}

void Utils::CartesianToPolar(const D3DXVECTOR2& pos, float& radius, float& angle)
{
    radius = sqrtf(pos.x * pos.x + pos.y * pos.y);
    angle = atan2f(pos.y, pos.x);
}

float Utils::Distance2D(const D3DXVECTOR2& a, const D3DXVECTOR2& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

D3DXVECTOR2 Utils::Normalize2D(const D3DXVECTOR2& v)
{
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length > 0.0f)
        return D3DXVECTOR2(v.x / length, v.y / length);
    return D3DXVECTOR2(0.0f, 0.0f);
}

D3DXVECTOR2 Utils::Rotate2D(const D3DXVECTOR2& v, float angle)
{
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    return D3DXVECTOR2(v.x * cosA - v.y * sinA, v.x * sinA + v.y * cosA);
}

D3DXVECTOR3 Utils::RGBToHSV(D3DCOLOR color)
{
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;

    float max_val = std::max({r, g, b});
    float min_val = std::min({r, g, b});
    float delta = max_val - min_val;

    D3DXVECTOR3 hsv;
    hsv.z = max_val; // Value

    if (max_val == 0.0f)
    {
        hsv.y = 0.0f; // Saturation
    }
    else
    {
        hsv.y = delta / max_val; // Saturation
    }

    if (delta == 0.0f)
    {
        hsv.x = 0.0f; // Hue
    }
    else
    {
        if (max_val == r)
            hsv.x = 60.0f * fmodf((g - b) / delta, 6.0f);
        else if (max_val == g)
            hsv.x = 60.0f * ((b - r) / delta + 2.0f);
        else
            hsv.x = 60.0f * ((r - g) / delta + 4.0f);

        if (hsv.x < 0.0f) hsv.x += 360.0f;
    }

    return hsv;
}

D3DCOLOR Utils::HSVToRGB(const D3DXVECTOR3& hsv)
{
    float c = hsv.z * hsv.y;
    float x = c * (1.0f - fabsf(fmodf(hsv.x / 60.0f, 2.0f) - 1.0f));
    float m = hsv.z - c;

    float r, g, b;

    if (hsv.x >= 0.0f && hsv.x < 60.0f)
    {
        r = c; g = x; b = 0.0f;
    }
    else if (hsv.x >= 60.0f && hsv.x < 120.0f)
    {
        r = x; g = c; b = 0.0f;
    }
    else if (hsv.x >= 120.0f && hsv.x < 180.0f)
    {
        r = 0.0f; g = c; b = x;
    }
    else if (hsv.x >= 180.0f && hsv.x < 240.0f)
    {
        r = 0.0f; g = x; b = c;
    }
    else if (hsv.x >= 240.0f && hsv.x < 300.0f)
    {
        r = x; g = 0.0f; b = c;
    }
    else
    {
        r = c; g = 0.0f; b = x;
    }

    BYTE nr = static_cast<BYTE>((r + m) * 255.0f);
    BYTE ng = static_cast<BYTE>((g + m) * 255.0f);
    BYTE nb = static_cast<BYTE>((b + m) * 255.0f);

    return D3DCOLOR_ARGB(255, nr, ng, nb);
}

float Utils::GetLuminance(D3DCOLOR color)
{
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;

    return 0.299f * r + 0.587f * g + 0.114f * b;
}

D3DCOLOR Utils::SampleBilinear(std::shared_ptr<Texture> texture, float u, float v)
{
    if (!texture) return D3DCOLOR_ARGB(0, 0, 0, 0);

    int width = texture->GetWidth();
    int height = texture->GetHeight();

    float x = u * (width - 1);
    float y = v * (height - 1);

    int x0 = static_cast<int>(floorf(x));
    int y0 = static_cast<int>(floorf(y));
    int x1 = std::min(x0 + 1, width - 1);
    int y1 = std::min(y0 + 1, height - 1);

    float fx = x - x0;
    float fy = y - y0;

    D3DCOLOR c00 = GetPixelSafe(texture, x0, y0);
    D3DCOLOR c01 = GetPixelSafe(texture, x0, y1);
    D3DCOLOR c10 = GetPixelSafe(texture, x1, y0);
    D3DCOLOR c11 = GetPixelSafe(texture, x1, y1);

    D3DCOLOR c0 = InterpolateColor(c00, c10, fx);
    D3DCOLOR c1 = InterpolateColor(c01, c11, fx);

    return InterpolateColor(c0, c1, fy);
}

float Utils::RandomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

int Utils::RandomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

D3DCOLOR Utils::RandomColor()
{
    BYTE r = static_cast<BYTE>(RandomInt(0, 255));
    BYTE g = static_cast<BYTE>(RandomInt(0, 255));
    BYTE b = static_cast<BYTE>(RandomInt(0, 255));
    return D3DCOLOR_ARGB(255, r, g, b);
}

// Easing functions
float Utils::EaseInQuad(float t)
{
    return t * t;
}

float Utils::EaseOutQuad(float t)
{
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float Utils::EaseInOutQuad(float t)
{
    return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
}

float Utils::EaseInCubic(float t)
{
    return t * t * t;
}

float Utils::EaseOutCubic(float t)
{
    float f = 1.0f - t;
    return 1.0f - f * f * f;
}

float Utils::EaseInOutCubic(float t)
{
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - 4.0f * (1.0f - t) * (1.0f - t) * (1.0f - t);
}

float Utils::EaseInSine(float t)
{
    return 1.0f - cosf(t * 3.14159f / 2.0f);
}

float Utils::EaseOutSine(float t)
{
    return sinf(t * 3.14159f / 2.0f);
}

float Utils::EaseInOutSine(float t)
{
    return -(cosf(3.14159f * t) - 1.0f) / 2.0f;
}

// Helper functions
D3DCOLOR Utils::GetPixelSafe(std::shared_ptr<Texture> texture, int x, int y)
{
    if (!texture) return D3DCOLOR_ARGB(0, 0, 0, 0);

    int width = texture->GetWidth();
    int height = texture->GetHeight();

    x = std::max(0, std::min(width - 1, x));
    y = std::max(0, std::min(height - 1, y));

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return D3DCOLOR_ARGB(0, 0, 0, 0);

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    D3DCOLOR color = pixels[y * pitch + x];

    texture->Unlock();
    return color;
}

} // namespace TextureEffects
