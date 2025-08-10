#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Textures/Material.h"
#include <iostream>

Renderer::Renderer()
    : m_d3d(nullptr)
    , m_device(nullptr)
    , m_deviceLost(false)
    , m_hwnd(nullptr)
    , m_width(0)
    , m_height(0)
    , m_fullscreen(false)
{
    // Inicializar matrices
    D3DXMatrixIdentity(&m_worldMatrix);
    D3DXMatrixIdentity(&m_viewMatrix);
    D3DXMatrixIdentity(&m_projectionMatrix);

    // Limpiar estadísticas
    memset(&m_stats, 0, sizeof(m_stats));
    memset(&m_frameStats, 0, sizeof(m_frameStats));
}

Renderer::~Renderer()
{
    Shutdown();
}

bool Renderer::Initialize(HWND hwnd, int width, int height, bool fullscreen)
{
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;
    m_fullscreen = fullscreen;

    // Crear objeto DirectX 9
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_d3d)
    {
        std::cerr << "Failed to create Direct3D object!" << std::endl;
        return false;
    }

    // Obtener capacidades del adaptador
    D3DCAPS9 caps;
    m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    // Verificar soporte de shaders
    if (caps.VertexShaderVersion < D3DVS_VERSION(3, 0) ||
        caps.PixelShaderVersion < D3DPS_VERSION(3, 0))
    {
        std::cerr << "Hardware does not support Shader Model 3.0!" << std::endl;
        std::cerr << "Vertex Shader: " << D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion)
                  << "." << D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion) << std::endl;
        std::cerr << "Pixel Shader: " << D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion)
                  << "." << D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion) << std::endl;
    }

    // Configurar parámetros de presentación
    ZeroMemory(&m_presentParams, sizeof(m_presentParams));
    m_presentParams.BackBufferWidth = width;
    m_presentParams.BackBufferHeight = height;
    m_presentParams.BackBufferFormat = fullscreen ? D3DFMT_X8R8G8B8 : D3DFMT_UNKNOWN;
    m_presentParams.BackBufferCount = 1;
    m_presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_presentParams.MultiSampleQuality = 0;
    m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_presentParams.hDeviceWindow = hwnd;
    m_presentParams.Windowed = !fullscreen;
    m_presentParams.EnableAutoDepthStencil = TRUE;
    m_presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_presentParams.Flags = 0;
    m_presentParams.FullScreen_RefreshRateInHz = fullscreen ? D3DPRESENT_RATE_DEFAULT : 0;
    m_presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Intentar crear dispositivo con hardware vertex processing
    DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        // Verificar soporte de vertex shaders en hardware
        if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 0))
        {
            behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        }
        else
        {
            behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
        }
    }
    else
    {
        behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    HRESULT hr = m_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        behaviorFlags,
        &m_presentParams,
        &m_device
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create Direct3D device! HRESULT: 0x" << std::hex << hr << std::endl;

        // Intentar con software vertex processing como fallback
        hr = m_d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &m_presentParams,
            &m_device
        );

        if (FAILED(hr))
        {
            std::cerr << "Failed to create Direct3D device with software VP! HRESULT: 0x"
                      << std::hex << hr << std::endl;
            return false;
        }

        std::cout << "Created device with software vertex processing" << std::endl;
    }
    else
    {
        std::cout << "Created device with hardware vertex processing" << std::endl;
    }

    // Configurar estados por defecto
    SetupDefaultStates();

    // Crear matrices
    CreateMatrices();

    std::cout << "Renderer initialized successfully!" << std::endl;
    std::cout << "Resolution: " << width << "x" << height << std::endl;
    std::cout << "Fullscreen: " << (fullscreen ? "Yes" : "No") << std::endl;

    return true;
}

void Renderer::SetupDefaultStates()
{
    // Estados de renderizado
    m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_device->SetRenderState(D3DRS_LIGHTING, FALSE); // Usaremos shaders
    m_device->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    m_device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    // Alpha blending
    m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Estados de textura por defecto
    for (int i = 0; i < 8; i++)
    {
        m_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    }
}

void Renderer::CreateMatrices()
{
    // Matriz de proyección por defecto
    D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, D3DX_PI / 4.0f,
                              (float)m_width / (float)m_height, 0.1f, 100.0f);

    m_device->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);
}

void Renderer::Shutdown()
{
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_d3d)
    {
        m_d3d->Release();
        m_d3d = nullptr;
    }
}

void Renderer::BeginFrame()
{
    // Limpiar estadísticas del frame
    memset(&m_frameStats, 0, sizeof(m_frameStats));

    // Verificar si el dispositivo está perdido
    if (m_deviceLost)
    {
        if (!ResetDevice())
            return;
    }
}

void Renderer::EndFrame()
{
    // Copiar estadísticas del frame
    m_stats = m_frameStats;
}

void Renderer::Present()
{
    if (!m_device)
        return;

    HRESULT hr = m_device->Present(nullptr, nullptr, nullptr, nullptr);

    if (hr == D3DERR_DEVICELOST)
    {
        m_deviceLost = true;
    }
}

void Renderer::Clear(D3DCOLOR color)
{
    if (!m_device)
        return;

    m_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                   color, 1.0f, 0);
}

void Renderer::ClearDepth()
{
    if (!m_device)
        return;

    m_device->Clear(0, nullptr, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}

void Renderer::ClearStencil()
{
    if (!m_device)
        return;

    m_device->Clear(0, nullptr, D3DCLEAR_STENCIL, 0, 1.0f, 0);
}

bool Renderer::CheckDeviceLost()
{
    if (!m_device)
        return true;

    HRESULT hr = m_device->TestCooperativeLevel();

    if (hr == D3DERR_DEVICELOST)
    {
        m_deviceLost = true;
        return true;
    }
    else if (hr == D3DERR_DEVICENOTRESET)
    {
        return ResetDevice();
    }

    return false;
}

bool Renderer::ResetDevice()
{
    if (!m_device)
        return false;

    HRESULT hr = m_device->Reset(&m_presentParams);

    if (SUCCEEDED(hr))
    {
        m_deviceLost = false;
        SetupDefaultStates();
        CreateMatrices();
        return true;
    }

    return false;
}

void Renderer::HandleDeviceLost()
{
    // Esperar hasta que el dispositivo pueda ser reseteado
    Sleep(50);
}

void Renderer::SetupMatrices(const Camera* camera)
{
    if (!camera)
        return;

    m_viewMatrix = camera->GetViewMatrix();
    m_projectionMatrix = camera->GetProjectionMatrix();

    m_device->SetTransform(D3DTS_VIEW, &m_viewMatrix);
    m_device->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);
}

void Renderer::RenderMesh(const Mesh* mesh, const Material* material, const D3DXMATRIX& worldMatrix)
{
    if (!mesh || !material || !m_device)
        return;

    // Establecer matriz del mundo
    SetWorldMatrix(worldMatrix);

    // Aplicar material
    material->Apply(m_device);

    // Renderizar mesh
    mesh->Render(m_device);

    // Actualizar estadísticas
    m_frameStats.drawCalls++;
    m_frameStats.triangles += mesh->GetTriangleCount();
    m_frameStats.vertices += mesh->GetVertexCount();
}

void Renderer::SetWorldMatrix(const D3DXMATRIX& matrix)
{
    m_worldMatrix = matrix;
    m_device->SetTransform(D3DTS_WORLD, &m_worldMatrix);
}

void Renderer::SetViewMatrix(const D3DXMATRIX& matrix)
{
    m_viewMatrix = matrix;
    m_device->SetTransform(D3DTS_VIEW, &m_viewMatrix);
}

void Renderer::SetProjectionMatrix(const D3DXMATRIX& matrix)
{
    m_projectionMatrix = matrix;
    m_device->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);
}

void Renderer::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
    if (m_device)
        m_device->SetRenderState(state, value);
}

void Renderer::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
    if (m_device)
        m_device->SetTextureStageState(stage, type, value);
}

void Renderer::SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
{
    if (m_device)
        m_device->SetSamplerState(sampler, type, value);
}

void Renderer::SetTexture(DWORD stage, IDirect3DTexture9* texture)
{
    if (m_device)
        m_device->SetTexture(stage, texture);
}

void Renderer::SetMultipleTextures(const std::vector<IDirect3DTexture9*>& textures)
{
    for (size_t i = 0; i < textures.size() && i < 8; i++)
    {
        SetTexture(static_cast<DWORD>(i), textures[i]);
    }
}

void Renderer::EnableAlphaBlending(bool enable)
{
    SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
}

void Renderer::SetBlendMode(D3DBLEND srcBlend, D3DBLEND destBlend)
{
    SetRenderState(D3DRS_SRCBLEND, srcBlend);
    SetRenderState(D3DRS_DESTBLEND, destBlend);
}

void Renderer::SetViewport(int x, int y, int width, int height)
{
    D3DVIEWPORT9 viewport;
    viewport.X = x;
    viewport.Y = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    if (m_device)
        m_device->SetViewport(&viewport);
}

D3DVIEWPORT9 Renderer::GetViewport() const
{
    D3DVIEWPORT9 viewport;
    if (m_device)
        m_device->GetViewport(&viewport);
    else
        memset(&viewport, 0, sizeof(viewport));

    return viewport;
}
