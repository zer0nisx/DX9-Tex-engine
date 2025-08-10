#include "TextureManager.h"
#include "Texture.h"
#include <iostream>
#include <algorithm>

TextureManager::TextureManager()
    : m_device(nullptr)
    , m_defaultFilter(TextureFilter::LINEAR)
    , m_defaultWrap(TextureWrap::REPEAT)
    , m_anisotropyLevel(4)
    , m_mipMapBias(0.0f)
    , m_poolSize(100)
    , m_totalMemoryUsage(0)
{
}

TextureManager::~TextureManager()
{
    Shutdown();
}

bool TextureManager::Initialize(IDirect3DDevice9* device)
{
    if (!device)
    {
        std::cerr << "Invalid device pointer!" << std::endl;
        return false;
    }

    m_device = device;

    std::cout << "TextureManager initialized successfully!" << std::endl;
    return true;
}

void TextureManager::Shutdown()
{
    UnloadAllTextures();
    m_device = nullptr;
}

std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& filename, TextureType type)
{
    std::string key = GetTextureKey(filename, type);

    // Verificar si ya está cargada
    auto it = m_textures.find(key);
    if (it != m_textures.end())
    {
        return it->second;
    }

    // Crear nueva textura
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateFromFile(m_device, filename, type))
    {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return nullptr;
    }

    // Almacenar en cache
    m_textures[key] = texture;

    std::cout << "Loaded texture: " << filename << " (Type: " << static_cast<int>(type) << ")" << std::endl;
    return texture;
}

std::shared_ptr<Texture> TextureManager::CreateProceduralTexture(const std::string& name, int width, int height, D3DFORMAT format)
{
    // Verificar si ya existe
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second;
    }

    // Crear textura vacía
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(m_device, width, height, format))
    {
        std::cerr << "Failed to create procedural texture: " << name << std::endl;
        return nullptr;
    }

    // Llenar con patrón de ajedrez simple
    CreateCheckerboardPattern(texture, width, height);

    // Almacenar
    m_textures[name] = texture;

    std::cout << "Created procedural texture: " << name << " (" << width << "x" << height << ")" << std::endl;
    return texture;
}

std::shared_ptr<Texture> TextureManager::CreateNoiseTexture(const std::string& name, int width, int height, float frequency, int octaves)
{
    // Verificar si ya existe
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second;
    }

    // Crear textura vacía
    auto texture = std::make_shared<Texture>();
    if (!texture->CreateEmpty(m_device, width, height, D3DFMT_A8R8G8B8))
    {
        std::cerr << "Failed to create noise texture: " << name << std::endl;
        return nullptr;
    }

    // Generar ruido
    CreateNoisePattern(texture, width, height, frequency, octaves);

    // Almacenar
    m_textures[name] = texture;

    std::cout << "Created noise texture: " << name << " (" << width << "x" << height << ")" << std::endl;
    return texture;
}

void TextureManager::CreateCheckerboardPattern(std::shared_ptr<Texture> texture, int width, int height)
{
    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    const int checkerSize = 32;
    DWORD color1 = D3DCOLOR_ARGB(255, 255, 255, 255); // Blanco
    DWORD color2 = D3DCOLOR_ARGB(255, 128, 128, 128); // Gris

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            bool checker = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            pixels[y * pitch + x] = checker ? color1 : color2;
        }
    }

    texture->Unlock();
}

void TextureManager::CreateNoisePattern(std::shared_ptr<Texture> texture, int width, int height, float frequency, int octaves)
{
    D3DLOCKED_RECT lockedRect;
    if (!texture->Lock(&lockedRect))
        return;

    DWORD* pixels = static_cast<DWORD*>(lockedRect.pBits);
    int pitch = lockedRect.Pitch / sizeof(DWORD);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Noise simple usando funciones trigonométricas
            float fx = static_cast<float>(x) / width * frequency;
            float fy = static_cast<float>(y) / height * frequency;

            float noise = 0.0f;
            float amplitude = 1.0f;

            for (int i = 0; i < octaves; i++)
            {
                noise += sin(fx * 6.28f) * cos(fy * 6.28f) * amplitude;
                fx *= 2.0f;
                fy *= 2.0f;
                amplitude *= 0.5f;
            }

            // Normalizar a [0, 1]
            noise = (noise + 1.0f) * 0.5f;
            noise = std::max(0.0f, std::min(1.0f, noise));

            BYTE intensity = static_cast<BYTE>(noise * 255);
            pixels[y * pitch + x] = D3DCOLOR_ARGB(255, intensity, intensity, intensity);
        }
    }

    texture->Unlock();
}

std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& name)
{
    auto it = m_textures.find(name);
    return (it != m_textures.end()) ? it->second : nullptr;
}

bool TextureManager::HasTexture(const std::string& name) const
{
    return m_textures.find(name) != m_textures.end();
}

void TextureManager::UnloadTexture(const std::string& name)
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        m_textures.erase(it);
        std::cout << "Unloaded texture: " << name << std::endl;
    }
}

void TextureManager::UnloadAllTextures()
{
    size_t count = m_textures.size();
    m_textures.clear();

    if (count > 0)
    {
        std::cout << "Unloaded " << count << " textures" << std::endl;
    }
}

void TextureManager::BindTextures(const std::vector<std::shared_ptr<Texture>>& textures, int startStage)
{
    for (size_t i = 0; i < textures.size() && (startStage + i) < 8; i++)
    {
        if (textures[i])
        {
            textures[i]->Bind(startStage + static_cast<int>(i));
        }
    }
}

void TextureManager::UnbindAllTextures()
{
    for (int i = 0; i < 8; i++)
    {
        m_device->SetTexture(i, nullptr);
    }
}

size_t TextureManager::GetTotalMemoryUsage() const
{
    size_t totalMemory = 0;
    for (const auto& pair : m_textures)
    {
        totalMemory += pair.second->GetMemoryUsage();
    }
    return totalMemory;
}

std::vector<TextureInfo> TextureManager::GetLoadedTextures() const
{
    std::vector<TextureInfo> infos;

    for (const auto& pair : m_textures)
    {
        TextureInfo info;
        info.filename = pair.second->GetFilename();
        info.type = pair.second->GetType();
        info.width = pair.second->GetWidth();
        info.height = pair.second->GetHeight();
        info.format = pair.second->GetFormat();
        info.memoryUsage = pair.second->GetMemoryUsage();
        infos.push_back(info);
    }

    return infos;
}

std::string TextureManager::GetTextureKey(const std::string& filename, TextureType type) const
{
    return filename + "_" + std::to_string(static_cast<int>(type));
}
