#include "PostEffects.h"
#include "../Texture.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace TextureEffects {

void PostEffects::AdjustBrightness(std::shared_ptr<Texture> texture, float brightness)
{
    if (!texture) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    brightness = std::max(-1.0f, std::min(1.0f, brightness));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            D3DCOLOR color = pixels[y * pitch + x];

            BYTE r = (color >> 16) & 0xFF;
            BYTE g = (color >> 8) & 0xFF;
            BYTE b = color & 0xFF;
            BYTE a = (color >> 24) & 0xFF;

            if (brightness > 0)
            {
                r = static_cast<BYTE>(r + (255 - r) * brightness);
                g = static_cast<BYTE>(g + (255 - g) * brightness);
                b = static_cast<BYTE>(b + (255 - b) * brightness);
            }
            else
            {
                r = static_cast<BYTE>(r * (1.0f + brightness));
                g = static_cast<BYTE>(g * (1.0f + brightness));
                b = static_cast<BYTE>(b * (1.0f + brightness));
            }

            pixels[y * pitch + x] = D3DCOLOR_ARGB(a, r, g, b);
        }
    }

    texture->Unlock();
}

void PostEffects::AdjustContrast(std::shared_ptr<Texture> texture, float contrast)
{
    if (!texture) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            D3DCOLOR color = pixels[y * pitch + x];

            BYTE r = (color >> 16) & 0xFF;
            BYTE g = (color >> 8) & 0xFF;
            BYTE b = color & 0xFF;
            BYTE a = (color >> 24) & 0xFF;

            r = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(factor * (r - 128) + 128))));
            g = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(factor * (g - 128) + 128))));
            b = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(factor * (b - 128) + 128))));

            pixels[y * pitch + x] = D3DCOLOR_ARGB(a, r, g, b);
        }
    }

    texture->Unlock();
}

void PostEffects::AdjustSaturation(std::shared_ptr<Texture> texture, float saturation)
{
    if (!texture) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            D3DCOLOR color = pixels[y * pitch + x];

            D3DXVECTOR3 hsv = RGBToHSV(color);
            hsv.y *= saturation; // Adjust saturation
            hsv.y = std::max(0.0f, std::min(1.0f, hsv.y));

            pixels[y * pitch + x] = HSVToRGB(hsv);
        }
    }

    texture->Unlock();
}

void PostEffects::ApplyBlur(std::shared_ptr<Texture> texture, float radius)
{
    if (!texture || radius <= 0.0f) return;

    int kernelSize = static_cast<int>(radius * 2) + 1;
    if (kernelSize % 2 == 0) kernelSize++;

    std::vector<float> kernel(kernelSize * kernelSize);
    float sum = 0.0f;

    // Generate Gaussian kernel
    int center = kernelSize / 2;
    for (int y = 0; y < kernelSize; y++)
    {
        for (int x = 0; x < kernelSize; x++)
        {
            float distance = sqrtf(static_cast<float>((x - center) * (x - center) + (y - center) * (y - center)));
            float value = expf(-(distance * distance) / (2.0f * radius * radius));
            kernel[y * kernelSize + x] = value;
            sum += value;
        }
    }

    // Normalize kernel
    for (float& value : kernel)
    {
        value /= sum;
    }

    ApplyKernel(texture, kernel.data(), kernelSize);
}

void PostEffects::ApplySharpen(std::shared_ptr<Texture> texture, float amount)
{
    if (!texture) return;

    float kernel[9] = {
        0.0f, -amount, 0.0f,
        -amount, 1.0f + 4.0f * amount, -amount,
        0.0f, -amount, 0.0f
    };

    ApplyKernel(texture, kernel, 3);
}

void PostEffects::ApplyEmboss(std::shared_ptr<Texture> texture, float strength, float angle)
{
    if (!texture) return;

    float rad = angle * 3.14159f / 180.0f;
    float cosA = cosf(rad) * strength;
    float sinA = sinf(rad) * strength;

    float kernel[9] = {
        -cosA - sinA, -sinA, cosA - sinA,
        -cosA, 1.0f, cosA,
        sinA - cosA, sinA, cosA + sinA
    };

    ApplyKernel(texture, kernel, 3);
}

void PostEffects::ApplyEdgeDetection(std::shared_ptr<Texture> texture, float threshold)
{
    if (!texture) return;

    // Sobel edge detection
    float sobelX[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    float sobelY[9] = {
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    };

    // Apply both kernels and combine
    ApplySobel(texture);
}

void PostEffects::AddNoise(std::shared_ptr<Texture> texture, float amount, bool monochrome)
{
    if (!texture || amount <= 0.0f) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-amount, amount);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            D3DCOLOR color = pixels[y * pitch + x];

            BYTE r = (color >> 16) & 0xFF;
            BYTE g = (color >> 8) & 0xFF;
            BYTE b = color & 0xFF;
            BYTE a = (color >> 24) & 0xFF;

            if (monochrome)
            {
                float noise = dis(gen) * 255.0f;
                r = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(r + noise))));
                g = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(g + noise))));
                b = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(b + noise))));
            }
            else
            {
                float noiseR = dis(gen) * 255.0f;
                float noiseG = dis(gen) * 255.0f;
                float noiseB = dis(gen) * 255.0f;

                r = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(r + noiseR))));
                g = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(g + noiseG))));
                b = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(b + noiseB))));
            }

            pixels[y * pitch + x] = D3DCOLOR_ARGB(a, r, g, b);
        }
    }

    texture->Unlock();
}

void PostEffects::Pixelate(std::shared_ptr<Texture> texture, int pixelSize)
{
    if (!texture || pixelSize <= 1) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    // Create a copy for sampling
    std::vector<DWORD> originalPixels(width * height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            originalPixels[y * width + x] = pixels[y * pitch + x];
        }
    }

    for (int y = 0; y < height; y += pixelSize)
    {
        for (int x = 0; x < width; x += pixelSize)
        {
            // Sample the color from the center of this pixel block
            int sampleX = std::min(x + pixelSize / 2, width - 1);
            int sampleY = std::min(y + pixelSize / 2, height - 1);
            D3DCOLOR blockColor = originalPixels[sampleY * width + sampleX];

            // Fill the entire block with this color
            for (int by = y; by < std::min(y + pixelSize, height); by++)
            {
                for (int bx = x; bx < std::min(x + pixelSize, width); bx++)
                {
                    pixels[by * pitch + bx] = blockColor;
                }
            }
        }
    }

    texture->Unlock();
}

// Helper functions
void PostEffects::ApplyKernel(std::shared_ptr<Texture> texture, const float* kernel, int kernelSize, float divisor)
{
    if (!texture) return;

    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect)) return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);
    int width = texture->GetWidth();
    int height = texture->GetHeight();

    // Create a copy for sampling
    std::vector<DWORD> originalPixels(width * height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            originalPixels[y * width + x] = pixels[y * pitch + x];
        }
    }

    int halfKernel = kernelSize / 2;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float r = 0.0f, g = 0.0f, b = 0.0f;

            for (int ky = 0; ky < kernelSize; ky++)
            {
                for (int kx = 0; kx < kernelSize; kx++)
                {
                    int px = std::max(0, std::min(width - 1, x + kx - halfKernel));
                    int py = std::max(0, std::min(height - 1, y + ky - halfKernel));

                    D3DCOLOR color = originalPixels[py * width + px];
                    float weight = kernel[ky * kernelSize + kx];

                    r += ((color >> 16) & 0xFF) * weight;
                    g += ((color >> 8) & 0xFF) * weight;
                    b += (color & 0xFF) * weight;
                }
            }

            r /= divisor;
            g /= divisor;
            b /= divisor;

            BYTE nr = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(r))));
            BYTE ng = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(g))));
            BYTE nb = static_cast<BYTE>(std::max(0, std::min(255, static_cast<int>(b))));
            BYTE a = (originalPixels[y * width + x] >> 24) & 0xFF;

            pixels[y * pitch + x] = D3DCOLOR_ARGB(a, nr, ng, nb);
        }
    }

    texture->Unlock();
}

void PostEffects::ApplySobel(std::shared_ptr<Texture> texture)
{
    // Simplified Sobel implementation - in practice would need more sophisticated edge detection
    float sobelKernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    ApplyKernel(texture, sobelKernel, 3);
}

// Color space conversion functions
D3DXVECTOR3 PostEffects::RGBToHSV(D3DCOLOR color)
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

D3DCOLOR PostEffects::HSVToRGB(const D3DXVECTOR3& hsv)
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

float PostEffects::GetLuminance(D3DCOLOR color)
{
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;

    return 0.299f * r + 0.587f * g + 0.114f * b;
}

} // namespace TextureEffects
