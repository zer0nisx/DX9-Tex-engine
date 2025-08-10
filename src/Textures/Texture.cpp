#include "Texture.h"
#include "../Core/Utils.h"
#include <iostream>

Texture::Texture()
    : m_device(nullptr)
    , m_texture(nullptr)
    , m_cubeTexture(nullptr)
    , m_volumeTexture(nullptr)
    , m_type(TextureType::DIFFUSE)
    , m_width(0)
    , m_height(0)
    , m_depth(0)
    , m_format(D3DFMT_UNKNOWN)
    , m_mipLevels(0)
    , m_memoryUsage(0)
    , m_isLocked(false)
{
    memset(&m_animationData, 0, sizeof(m_animationData));
    m_animationData.scaleU = 1.0f;
    m_animationData.scaleV = 1.0f;
}

Texture::~Texture()
{
    Release();
}

bool Texture::CreateFromFile(IDirect3DDevice9* device, const std::string& filename, TextureType type)
{
    if (!device)
    {
        std::cerr << "Invalid device pointer!" << std::endl;
        return false;
    }

    m_device = device;
    m_filename = filename;
    m_type = type;

    // Cargar textura desde archivo usando D3DX
    std::wstring wFilename = StringToWString(filename);
    HRESULT hr = D3DXCreateTextureFromFileExW(
        device,
        wFilename.c_str(),
        D3DX_DEFAULT_NONPOW2,  // width
        D3DX_DEFAULT_NONPOW2,  // height
        D3DX_DEFAULT,          // mip levels
        0,                     // usage
        D3DFMT_FROM_FILE,      // format
        D3DPOOL_MANAGED,       // pool
        D3DX_DEFAULT,          // filter
        D3DX_DEFAULT,          // mip filter
        0,                     // color key
        nullptr,               // src info
        nullptr,               // palette
        &m_texture
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to load texture from file: " << filename << " (HRESULT: 0x"
                  << std::hex << hr << ")" << std::endl;
        return false;
    }

    // Obtener información de la textura
    D3DSURFACE_DESC desc;
    m_texture->GetLevelDesc(0, &desc);

    m_width = desc.Width;
    m_height = desc.Height;
    m_format = desc.Format;
    m_mipLevels = m_texture->GetLevelCount();

    CalculateMemoryUsage();

    std::cout << "Loaded texture: " << filename << " (" << m_width << "x" << m_height << ")" << std::endl;
    return true;
}

bool Texture::CreateEmpty(IDirect3DDevice9* device, int width, int height, D3DFORMAT format, int mipLevels)
{
    if (!device)
    {
        std::cerr << "Invalid device pointer!" << std::endl;
        return false;
    }

    m_device = device;
    m_width = width;
    m_height = height;
    m_format = format;
    m_mipLevels = (mipLevels == 0) ? 1 : mipLevels;

    HRESULT hr = device->CreateTexture(
        width,
        height,
        m_mipLevels,
        0,                   // usage
        format,
        D3DPOOL_MANAGED,
        &m_texture,
        nullptr
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create empty texture! (HRESULT: 0x" << std::hex << hr << ")" << std::endl;
        return false;
    }

    CalculateMemoryUsage();
    return true;
}

void Texture::Bind(int stage) const
{
    if (!m_device)
        return;

    if (m_texture)
    {
        m_device->SetTexture(stage, m_texture);
    }
    else if (m_cubeTexture)
    {
        m_device->SetTexture(stage, m_cubeTexture);
    }
    else if (m_volumeTexture)
    {
        m_device->SetTexture(stage, m_volumeTexture);
    }
}

void Texture::Unbind(int stage) const
{
    if (m_device)
    {
        m_device->SetTexture(stage, nullptr);
    }
}

bool Texture::Lock(D3DLOCKED_RECT* lockedRect, const RECT* rect, DWORD flags)
{
    if (!m_texture || m_isLocked)
        return false;

    HRESULT hr = m_texture->LockRect(0, lockedRect, rect, flags);
    if (SUCCEEDED(hr))
    {
        m_isLocked = true;
        return true;
    }

    return false;
}

void Texture::Unlock()
{
    if (m_texture && m_isLocked)
    {
        m_texture->UnlockRect(0);
        m_isLocked = false;
    }
}

bool Texture::GenerateMipmaps()
{
    if (!m_texture)
        return false;

    // DirectX 9 genera mipmaps automáticamente si se especifica D3DX_DEFAULT en mip levels
    // Para texturas ya creadas, usamos D3DXFilterTexture
    HRESULT hr = D3DXFilterTexture(m_texture, nullptr, D3DX_DEFAULT, D3DX_DEFAULT);
    return SUCCEEDED(hr);
}

void Texture::SetFilter(TextureFilter filter)
{
    if (!m_device)
        return;

    D3DTEXTUREFILTERTYPE d3dFilter = ConvertFilter(filter);

    // Aplicar a todos los stages donde esta textura esté activa
    // Esto es simplificado - en un sistema real, rastrearíamos qué stage usa esta textura
    for (int i = 0; i < 8; i++)
    {
        m_device->SetSamplerState(i, D3DSAMP_MINFILTER, d3dFilter);
        m_device->SetSamplerState(i, D3DSAMP_MAGFILTER, d3dFilter);
        m_device->SetSamplerState(i, D3DSAMP_MIPFILTER, d3dFilter);
    }
}

void Texture::SetWrap(TextureWrap wrap)
{
    if (!m_device)
        return;

    D3DTEXTUREADDRESS d3dWrap = ConvertWrap(wrap);

    for (int i = 0; i < 8; i++)
    {
        m_device->SetSamplerState(i, D3DSAMP_ADDRESSU, d3dWrap);
        m_device->SetSamplerState(i, D3DSAMP_ADDRESSV, d3dWrap);
    }
}

void Texture::UpdateAnimation(float deltaTime)
{
    m_animationData.time += deltaTime;

    // Actualizar scroll UV
    // Esto se aplicaría en el shader o en el sistema de materiales
}

void Texture::Release()
{
    if (m_texture)
    {
        m_texture->Release();
        m_texture = nullptr;
    }

    if (m_cubeTexture)
    {
        m_cubeTexture->Release();
        m_cubeTexture = nullptr;
    }

    if (m_volumeTexture)
    {
        m_volumeTexture->Release();
        m_volumeTexture = nullptr;
    }

    m_device = nullptr;
    m_isLocked = false;
}

void Texture::CalculateMemoryUsage()
{
    if (!m_texture)
    {
        m_memoryUsage = 0;
        return;
    }

    // Cálculo aproximado del uso de memoria
    int bytesPerPixel = 4; // Asumimos ARGB por defecto

    switch (m_format)
    {
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
        bytesPerPixel = 2;
        break;
    case D3DFMT_R8G8B8:
        bytesPerPixel = 3;
        break;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        bytesPerPixel = 4;
        break;
    case D3DFMT_DXT1:
        bytesPerPixel = 1; // Comprimido
        break;
    case D3DFMT_DXT3:
    case D3DFMT_DXT5:
        bytesPerPixel = 1; // Comprimido
        break;
    default:
        bytesPerPixel = 4;
        break;
    }

    m_memoryUsage = m_width * m_height * bytesPerPixel;

    // Añadir memoria de mipmaps (aproximadamente 1/3 adicional)
    if (m_mipLevels > 1)
    {
        m_memoryUsage = static_cast<size_t>(m_memoryUsage * 1.33f);
    }
}

D3DTEXTUREFILTERTYPE Texture::ConvertFilter(TextureFilter filter) const
{
    switch (filter)
    {
    case TextureFilter::POINT:
        return D3DTEXF_POINT;
    case TextureFilter::LINEAR:
        return D3DTEXF_LINEAR;
    case TextureFilter::ANISOTROPIC:
        return D3DTEXF_ANISOTROPIC;
    default:
        return D3DTEXF_LINEAR;
    }
}

D3DTEXTUREADDRESS Texture::ConvertWrap(TextureWrap wrap) const
{
    switch (wrap)
    {
    case TextureWrap::REPEAT:
        return D3DTADDRESS_WRAP;
    case TextureWrap::CLAMP:
        return D3DTADDRESS_CLAMP;
    case TextureWrap::MIRROR:
        return D3DTADDRESS_MIRROR;
    default:
        return D3DTADDRESS_WRAP;
    }
}
