#pragma once

#include <d3d9.h>
#include <memory>

class Texture;

namespace TextureEffects {

    class ProceduralTextures {
    public:
        // Basic patterns
        static std::shared_ptr<Texture> CreateCheckerboard(IDirect3DDevice9* device, int width, int height,
                                                          int checkerSize, D3DCOLOR color1, D3DCOLOR color2);
        static std::shared_ptr<Texture> CreateStripes(IDirect3DDevice9* device, int width, int height,
                                                     int stripeWidth, D3DCOLOR color1, D3DCOLOR color2, bool vertical = false);
        static std::shared_ptr<Texture> CreateGradient(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR startColor, D3DCOLOR endColor, bool radial = false);
        static std::shared_ptr<Texture> CreateCircles(IDirect3DDevice9* device, int width, int height,
                                                     int circleSize, D3DCOLOR color1, D3DCOLOR color2);
        static std::shared_ptr<Texture> CreatePolkaDots(IDirect3DDevice9* device, int width, int height,
                                                       int dotRadius, int spacing, D3DCOLOR dotColor, D3DCOLOR bgColor);

        // Noise textures
        static std::shared_ptr<Texture> CreatePerlinNoise(IDirect3DDevice9* device, int width, int height,
                                                         float frequency = 4.0f, int octaves = 4);
        static std::shared_ptr<Texture> CreateTurbulence(IDirect3DDevice9* device, int width, int height,
                                                        float frequency = 4.0f, int octaves = 6);
        static std::shared_ptr<Texture> CreateClouds(IDirect3DDevice9* device, int width, int height,
                                                    float frequency = 2.0f, int octaves = 5);
        static std::shared_ptr<Texture> CreateVoronoi(IDirect3DDevice9* device, int width, int height,
                                                     float frequency = 8.0f, D3DCOLOR color1 = D3DCOLOR_XRGB(255, 255, 255),
                                                     D3DCOLOR color2 = D3DCOLOR_XRGB(0, 0, 0));

        // Material textures
        static std::shared_ptr<Texture> CreateWoodGrain(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR lightWood, D3DCOLOR darkWood);
        static std::shared_ptr<Texture> CreateMarble(IDirect3DDevice9* device, int width, int height,
                                                    D3DCOLOR baseColor, D3DCOLOR veinColor);
        static std::shared_ptr<Texture> CreateMetal(IDirect3DDevice9* device, int width, int height,
                                                   D3DCOLOR metalColor, float roughness = 0.2f);
        static std::shared_ptr<Texture> CreateBrick(IDirect3DDevice9* device, int width, int height,
                                                   D3DCOLOR brickColor, D3DCOLOR mortarColor, int brickWidth = 64, int brickHeight = 32);
        static std::shared_ptr<Texture> CreateFabric(IDirect3DDevice9* device, int width, int height,
                                                    D3DCOLOR fabricColor, int threadDensity = 32);

        // Organic textures
        static std::shared_ptr<Texture> CreateSkin(IDirect3DDevice9* device, int width, int height,
                                                 D3DCOLOR baseColor, float roughness = 0.3f);
        static std::shared_ptr<Texture> CreateLeather(IDirect3DDevice9* device, int width, int height,
                                                     D3DCOLOR leatherColor, float grainSize = 0.5f);
        static std::shared_ptr<Texture> CreateRock(IDirect3DDevice9* device, int width, int height,
                                                  D3DCOLOR rockColor, float roughness = 0.8f);

        // Special effects
        static std::shared_ptr<Texture> CreateElectric(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR electricColor, float intensity = 1.0f);
        static std::shared_ptr<Texture> CreateCaustics(IDirect3DDevice9* device, int width, int height,
                                                      D3DCOLOR waterColor, float time = 0.0f);

    private:
        // Helper functions
        static float CalculateDistance(float x1, float y1, float x2, float y2);
        static D3DCOLOR BlendColors(D3DCOLOR color1, D3DCOLOR color2, float blend);
        static void FillSolidColor(std::shared_ptr<Texture> texture, D3DCOLOR color);
    };

}
