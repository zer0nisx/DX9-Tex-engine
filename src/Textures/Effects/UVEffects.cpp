#include "UVEffects.h"
#include <cmath>

namespace TextureEffects {

D3DXMATRIX UVEffects::CreateScrollMatrix(float offsetU, float offsetV)
{
    D3DXMATRIX matrix;
    D3DXMatrixTranslation(&matrix, offsetU, offsetV, 0.0f);
    return matrix;
}

D3DXMATRIX UVEffects::CreateScaleMatrix(float scaleU, float scaleV)
{
    D3DXMATRIX matrix;
    D3DXMatrixScaling(&matrix, scaleU, scaleV, 1.0f);
    return matrix;
}

D3DXMATRIX UVEffects::CreateRotationMatrix(float angle)
{
    D3DXMATRIX matrix;
    D3DXMatrixRotationZ(&matrix, angle);
    return matrix;
}

D3DXMATRIX UVEffects::CreateTranslationMatrix(float u, float v)
{
    return CreateScrollMatrix(u, v);
}

D3DXMATRIX UVEffects::CreateSinWaveMatrix(float time, float frequency, float amplitude, bool horizontal)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    float offset = sinf(time * frequency) * amplitude;

    if (horizontal)
    {
        matrix._31 = offset; // U offset based on V coordinate
    }
    else
    {
        matrix._32 = offset; // V offset based on U coordinate
    }

    return matrix;
}

D3DXMATRIX UVEffects::CreatePulseMatrix(float time, float frequency, float minScale, float maxScale)
{
    float scale = minScale + (maxScale - minScale) * (sinf(time * frequency) * 0.5f + 0.5f);
    return CreateScaleMatrix(scale, scale);
}

D3DXMATRIX UVEffects::CreateOrbitalMatrix(float time, float speed, float radius, const D3DXVECTOR2& center)
{
    float angle = time * speed;
    float offsetU = center.x + cosf(angle) * radius - center.x;
    float offsetV = center.y + sinf(angle) * radius - center.y;

    return CreateScrollMatrix(offsetU, offsetV);
}

D3DXMATRIX UVEffects::CreateSwirlMatrix(float time, float intensity, const D3DXVECTOR2& center)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // This is a simplified swirl - for true swirl effect, you'd need custom shader
    float angle = sinf(time) * intensity;

    // Translate to origin
    D3DXMATRIX translate1, rotate, translate2;
    D3DXMatrixTranslation(&translate1, -center.x, -center.y, 0.0f);
    D3DXMatrixRotationZ(&rotate, angle);
    D3DXMatrixTranslation(&translate2, center.x, center.y, 0.0f);

    matrix = translate1 * rotate * translate2;

    return matrix;
}

D3DXMATRIX UVEffects::CreateRippleMatrix(float time, float frequency, float amplitude, const D3DXVECTOR2& center)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // Simplified ripple effect - displacement based on distance from center
    float ripple = sinf(time * frequency) * amplitude;

    // This would typically require per-pixel calculation in a shader
    // Here we just apply a simple time-based transformation
    matrix._31 = ripple * sinf(time * frequency);
    matrix._32 = ripple * cosf(time * frequency);

    return matrix;
}

D3DXMATRIX UVEffects::CreateTurbulenceMatrix(float time, float intensity)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // Create chaotic movement
    float turbulenceU = sinf(time * 3.7f) * cosf(time * 2.3f) * intensity;
    float turbulenceV = cosf(time * 4.1f) * sinf(time * 1.9f) * intensity;

    matrix._31 = turbulenceU;
    matrix._32 = turbulenceV;

    return matrix;
}

D3DXMATRIX UVEffects::CreateWarpMatrix(float time, float strength, float frequency)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // Create warping effect
    float warpU = sinf(time * frequency) * strength;
    float warpV = cosf(time * frequency * 1.3f) * strength;

    matrix._31 = warpU;
    matrix._32 = warpV;

    return matrix;
}

D3DXMATRIX UVEffects::CreatePerspectiveMatrix(float fovY, float aspect)
{
    D3DXMATRIX matrix;
    D3DXMatrixPerspectiveFovLH(&matrix, fovY, aspect, 0.1f, 100.0f);
    return matrix;
}

D3DXMATRIX UVEffects::CreateFisheyeMatrix(float strength, const D3DXVECTOR2& center)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // Simplified fisheye - would need shader for proper implementation
    float scale = 1.0f + strength;
    matrix._11 = scale;
    matrix._22 = scale;

    return matrix;
}

D3DXMATRIX UVEffects::CreateBarrelDistortionMatrix(float k1, float k2, const D3DXVECTOR2& center)
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);

    // Simplified barrel distortion
    // Real implementation would require per-pixel shader calculation
    float distortion = 1.0f + k1 + k2;
    matrix._11 = distortion;
    matrix._22 = distortion;

    return matrix;
}

D3DXMATRIX UVEffects::CombineMatrices(const D3DXMATRIX& mat1, const D3DXMATRIX& mat2)
{
    D3DXMATRIX result;
    D3DXMatrixMultiply(&result, &mat1, &mat2);
    return result;
}

D3DXVECTOR2 UVEffects::TransformUV(const D3DXVECTOR2& uv, const D3DXMATRIX& matrix)
{
    D3DXVECTOR4 uvTransformed;
    D3DXVECTOR4 uvInput(uv.x, uv.y, 0.0f, 1.0f);

    D3DXVec4Transform(&uvTransformed, &uvInput, &matrix);

    return D3DXVECTOR2(uvTransformed.x, uvTransformed.y);
}

D3DXMATRIX UVEffects::CreateIdentityMatrix()
{
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);
    return matrix;
}

// Preset combinations
D3DXMATRIX UVEffects::CreateScrollingCloudsMatrix(float time)
{
    D3DXMATRIX scroll1 = CreateScrollMatrix(time * 0.01f, time * 0.005f);
    D3DXMATRIX scale = CreateScaleMatrix(1.5f, 1.5f);
    D3DXMATRIX wave = CreateSinWaveMatrix(time, 0.5f, 0.02f, true);

    return CombineMatrices(CombineMatrices(scroll1, scale), wave);
}

D3DXMATRIX UVEffects::CreateWaterSurfaceMatrix(float time)
{
    D3DXMATRIX scroll = CreateScrollMatrix(time * 0.02f, time * 0.01f);
    D3DXMATRIX wave1 = CreateSinWaveMatrix(time, 2.0f, 0.01f, true);
    D3DXMATRIX wave2 = CreateSinWaveMatrix(time * 1.3f, 1.5f, 0.008f, false);

    return CombineMatrices(CombineMatrices(scroll, wave1), wave2);
}

D3DXMATRIX UVEffects::CreateFireFlickerMatrix(float time)
{
    D3DXMATRIX turbulence = CreateTurbulenceMatrix(time, 0.005f);
    D3DXMATRIX scroll = CreateScrollMatrix(0.0f, time * 0.1f); // Upward movement
    D3DXMATRIX flicker = CreateSinWaveMatrix(time, 8.0f, 0.002f, true);

    return CombineMatrices(CombineMatrices(turbulence, scroll), flicker);
}

D3DXMATRIX UVEffects::CreateElectricDistortionMatrix(float time)
{
    D3DXMATRIX distortion = CreateTurbulenceMatrix(time, 0.02f);
    D3DXMATRIX pulse = CreatePulseMatrix(time, 6.0f, 0.98f, 1.02f);

    return CombineMatrices(distortion, pulse);
}

} // namespace TextureEffects
