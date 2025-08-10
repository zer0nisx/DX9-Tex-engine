#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "TextureManager.h"

class Texture {
public:
    Texture();
    ~Texture();

    // Creation
    bool CreateFromFile(IDirect3DDevice9* device, const std::string& filename, TextureType type);
    bool CreateEmpty(IDirect3DDevice9* device, int width, int height, D3DFORMAT format, int mipLevels = 1);
    bool CreateCubeMap(IDirect3DDevice9* device, const std::string& filename);
    bool CreateVolumeTexture(IDirect3DDevice9* device, const std::string& filename);

    // Binding
    void Bind(int stage = 0) const;
    void Unbind(int stage = 0) const;

    // Properties
    IDirect3DTexture9* GetD3DTexture() const { return m_texture; }
    IDirect3DCubeTexture9* GetD3DCubeTexture() const { return m_cubeTexture; }
    IDirect3DVolumeTexture9* GetD3DVolumeTexture() const { return m_volumeTexture; }

    const std::string& GetFilename() const { return m_filename; }
    TextureType GetType() const { return m_type; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetDepth() const { return m_depth; }
    D3DFORMAT GetFormat() const { return m_format; }
    int GetMipLevels() const { return m_mipLevels; }
    size_t GetMemoryUsage() const { return m_memoryUsage; }

    // Texture operations
    bool Lock(D3DLOCKED_RECT* lockedRect, const RECT* rect = nullptr, DWORD flags = 0);
    void Unlock();
    bool SaveToFile(const std::string& filename) const;
    bool GenerateMipmaps();

    // Data access
    bool GetPixelData(std::vector<DWORD>& data) const;
    bool SetPixelData(const std::vector<DWORD>& data);
    DWORD GetPixel(int x, int y) const;
    void SetPixel(int x, int y, DWORD color);

    // Animation support
    struct AnimationData {
        float scrollU = 0.0f;
        float scrollV = 0.0f;
        float scaleU = 1.0f;
        float scaleV = 1.0f;
        float rotation = 0.0f;
        float time = 0.0f;
    };

    void SetAnimationData(const AnimationData& data) { m_animationData = data; }
    const AnimationData& GetAnimationData() const { return m_animationData; }
    void UpdateAnimation(float deltaTime);

    // Filtering and wrapping
    void SetFilter(TextureFilter filter);
    void SetWrap(TextureWrap wrap);
    void SetAnisotropy(int level);

    // State management
    bool IsValid() const { return m_texture != nullptr || m_cubeTexture != nullptr || m_volumeTexture != nullptr; }
    bool IsLocked() const { return m_isLocked; }
    void Release();

private:
    void CalculateMemoryUsage();
    D3DTEXTUREFILTERTYPE ConvertFilter(TextureFilter filter) const;
    D3DTEXTUREADDRESS ConvertWrap(TextureWrap wrap) const;

    IDirect3DDevice9* m_device;
    IDirect3DTexture9* m_texture;
    IDirect3DCubeTexture9* m_cubeTexture;
    IDirect3DVolumeTexture9* m_volumeTexture;

    std::string m_filename;
    TextureType m_type;
    int m_width;
    int m_height;
    int m_depth;
    D3DFORMAT m_format;
    int m_mipLevels;
    size_t m_memoryUsage;

    bool m_isLocked;
    AnimationData m_animationData;
};
