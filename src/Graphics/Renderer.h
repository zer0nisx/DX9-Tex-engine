#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <memory>
#include <vector>

// Forward declarations
class Material;
class Mesh;
class Camera;

struct RenderStats {
    int drawCalls = 0;
    int triangles = 0;
    int vertices = 0;
    float frameTime = 0.0f;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization
    bool Initialize(HWND hwnd, int width, int height, bool fullscreen = false);
    void Shutdown();

    // Frame rendering
    void BeginFrame();
    void EndFrame();
    void Present();

    // Clear operations
    void Clear(D3DCOLOR color = D3DCOLOR_XRGB(0, 0, 0));
    void ClearDepth();
    void ClearStencil();

    // Device management
    bool CheckDeviceLost();
    bool ResetDevice();
    void HandleDeviceLost();

    // Rendering
    void SetupMatrices(const Camera* camera);
    void RenderMesh(const Mesh* mesh, const Material* material, const D3DXMATRIX& worldMatrix);
    void SetWorldMatrix(const D3DXMATRIX& matrix);
    void SetViewMatrix(const D3DXMATRIX& matrix);
    void SetProjectionMatrix(const D3DXMATRIX& matrix);

    // States
    void SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
    void SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);
    void SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);

    // Textures
    void SetTexture(DWORD stage, IDirect3DTexture9* texture);
    void SetMultipleTextures(const std::vector<IDirect3DTexture9*>& textures);

    // Alpha blending
    void EnableAlphaBlending(bool enable);
    void SetBlendMode(D3DBLEND srcBlend, D3DBLEND destBlend);

    // Getters
    IDirect3DDevice9* GetDevice() const { return m_device; }
    const RenderStats& GetStats() const { return m_stats; }
    bool IsDeviceLost() const { return m_deviceLost; }

    // Viewport
    void SetViewport(int x, int y, int width, int height);
    D3DVIEWPORT9 GetViewport() const;

private:
    void SetupDefaultStates();
    void CreateMatrices();

    // DirectX objects
    IDirect3D9* m_d3d;
    IDirect3DDevice9* m_device;
    D3DPRESENT_PARAMETERS m_presentParams;

    // Device state
    bool m_deviceLost;
    HWND m_hwnd;
    int m_width;
    int m_height;
    bool m_fullscreen;

    // Matrices
    D3DXMATRIX m_worldMatrix;
    D3DXMATRIX m_viewMatrix;
    D3DXMATRIX m_projectionMatrix;

    // Render statistics
    RenderStats m_stats;
    mutable RenderStats m_frameStats;
};
