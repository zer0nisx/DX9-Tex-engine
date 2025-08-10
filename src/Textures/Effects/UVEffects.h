#pragma once

#include <d3d9.h>
#include <d3dx9.h>

namespace TextureEffects {

    class UVEffects {
    public:
        // Basic UV transformations
        static D3DXMATRIX CreateScrollMatrix(float offsetU, float offsetV);
        static D3DXMATRIX CreateScaleMatrix(float scaleU, float scaleV);
        static D3DXMATRIX CreateRotationMatrix(float angle);
        static D3DXMATRIX CreateTranslationMatrix(float u, float v);

        // Animated transformations
        static D3DXMATRIX CreateSinWaveMatrix(float time, float frequency, float amplitude, bool horizontal = true);
        static D3DXMATRIX CreatePulseMatrix(float time, float frequency, float minScale = 0.8f, float maxScale = 1.2f);
        static D3DXMATRIX CreateOrbitalMatrix(float time, float speed, float radius, const D3DXVECTOR2& center);

        // Complex transformations
        static D3DXMATRIX CreateSwirlMatrix(float time, float intensity, const D3DXVECTOR2& center);
        static D3DXMATRIX CreateRippleMatrix(float time, float frequency, float amplitude, const D3DXVECTOR2& center);
        static D3DXMATRIX CreateTurbulenceMatrix(float time, float intensity);
        static D3DXMATRIX CreateWarpMatrix(float time, float strength, float frequency);

        // Perspective and projection effects
        static D3DXMATRIX CreatePerspectiveMatrix(float fovY, float aspect);
        static D3DXMATRIX CreateFisheyeMatrix(float strength, const D3DXVECTOR2& center);
        static D3DXMATRIX CreateBarrelDistortionMatrix(float k1, float k2, const D3DXVECTOR2& center);

        // Utility functions
        static D3DXMATRIX CombineMatrices(const D3DXMATRIX& mat1, const D3DXMATRIX& mat2);
        static D3DXVECTOR2 TransformUV(const D3DXVECTOR2& uv, const D3DXMATRIX& matrix);
        static D3DXMATRIX CreateIdentityMatrix();

        // Preset combinations
        static D3DXMATRIX CreateScrollingCloudsMatrix(float time);
        static D3DXMATRIX CreateWaterSurfaceMatrix(float time);
        static D3DXMATRIX CreateFireFlickerMatrix(float time);
        static D3DXMATRIX CreateElectricDistortionMatrix(float time);
    };

}
