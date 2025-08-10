#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class Texture;

enum class TextureType {
    DIFFUSE = 0,
    NORMAL,
    SPECULAR,
    METALLIC,
    ROUGHNESS,
    AMBIENT_OCCLUSION,
    EMISSION,
    DISPLACEMENT,
    CUBE_MAP,
    VOLUME,
    COUNT
};

enum class TextureFilter {
    POINT,
    LINEAR,
    ANISOTROPIC
};

enum class TextureWrap {
    REPEAT,
    CLAMP,
    MIRROR
};

struct TextureInfo {
    std::string filename;
    TextureType type;
    int width = 0;
    int height = 0;
    int mipLevels = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    size_t memoryUsage = 0;
};

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    bool Initialize(IDirect3DDevice9* device);
    void Shutdown();

    // Texture loading
    std::shared_ptr<Texture> LoadTexture(const std::string& filename, TextureType type = TextureType::DIFFUSE);
    std::shared_ptr<Texture> LoadCubeTexture(const std::string& filename);
    std::shared_ptr<Texture> LoadVolumeTexture(const std::string& filename);

    // Procedural textures
    std::shared_ptr<Texture> CreateProceduralTexture(const std::string& name, int width, int height,
                                                     D3DFORMAT format = D3DFMT_A8R8G8B8);
    std::shared_ptr<Texture> CreateNoiseTexture(const std::string& name, int width, int height,
                                                float frequency = 1.0f, int octaves = 4);
    std::shared_ptr<Texture> CreateGradientTexture(const std::string& name, int width, int height,
                                                   D3DCOLOR color1, D3DCOLOR color2, bool horizontal = true);

    // Texture management
    void UnloadTexture(const std::string& name);
    void UnloadAllTextures();
    std::shared_ptr<Texture> GetTexture(const std::string& name);
    bool HasTexture(const std::string& name) const;

    // Texture operations
    bool SaveTexture(const std::shared_ptr<Texture>& texture, const std::string& filename);
    bool CompressTexture(const std::string& name, D3DFORMAT compressedFormat);
    bool GenerateMipmaps(const std::string& name);

    // Batch operations
    void SetTextureFilter(TextureFilter filter);
    void SetTextureWrap(TextureWrap wrap);
    void SetAnisotropyLevel(int level);
    void SetMipMapBias(float bias);

    // Multi-texturing
    void BindTextures(const std::vector<std::shared_ptr<Texture>>& textures, int startStage = 0);
    void UnbindAllTextures();

    // Statistics
    size_t GetTextureCount() const { return m_textures.size(); }
    size_t GetTotalMemoryUsage() const;
    std::vector<TextureInfo> GetLoadedTextures() const;

    // Preloading
    void PreloadTextures(const std::vector<std::string>& filenames);
    void SetTexturePoolSize(size_t poolSize) { m_poolSize = poolSize; }

private:
    std::string GetTextureKey(const std::string& filename, TextureType type) const;
    bool LoadTextureFromFile(const std::string& filename, IDirect3DTexture9** texture,
                            int* width = nullptr, int* height = nullptr);
    void CleanupUnusedTextures();
    void CreateCheckerboardPattern(std::shared_ptr<Texture> texture, int width, int height);
    void CreateNoisePattern(std::shared_ptr<Texture> texture, int width, int height, float frequency, int octaves);

    IDirect3DDevice9* m_device;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

    // Settings
    TextureFilter m_defaultFilter;
    TextureWrap m_defaultWrap;
    int m_anisotropyLevel;
    float m_mipMapBias;
    size_t m_poolSize;

    // Statistics
    mutable size_t m_totalMemoryUsage;
};
