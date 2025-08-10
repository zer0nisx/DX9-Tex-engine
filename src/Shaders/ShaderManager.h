#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class Effect;

struct ShaderInfo {
    std::string filename;
    std::string entryPoint;
    std::string profile;
    size_t memoryUsage = 0;
    bool isCompiled = false;
};

enum class ShaderType {
    VERTEX_SHADER,
    PIXEL_SHADER,
    EFFECT
};

struct ShaderMacro {
    std::string name;
    std::string definition;
};

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    bool Initialize(IDirect3DDevice9* device);
    void Shutdown();

    // Effect management
    std::shared_ptr<Effect> LoadEffect(const std::string& filename,
                                      const std::vector<ShaderMacro>& macros = {});
    std::shared_ptr<Effect> GetEffect(const std::string& name);
    void UnloadEffect(const std::string& name);
    void UnloadAllEffects();

    // Individual shader management
    IDirect3DVertexShader9* LoadVertexShader(const std::string& filename,
                                             const std::string& entryPoint = "VS",
                                             const std::string& profile = "vs_3_0");
    IDirect3DPixelShader9* LoadPixelShader(const std::string& filename,
                                          const std::string& entryPoint = "PS",
                                          const std::string& profile = "ps_3_0");

    // Compilation
    bool CompileShader(const std::string& source, const std::string& entryPoint,
                      const std::string& profile, ID3DXBuffer** shaderBuffer,
                      const std::vector<ShaderMacro>& macros = {});
    bool CompileEffect(const std::string& source, ID3DXEffect** effect,
                      const std::vector<ShaderMacro>& macros = {});

    // Hot reloading
    void EnableHotReload(bool enable) { m_hotReloadEnabled = enable; }
    void CheckForShaderChanges();
    void ReloadShader(const std::string& filename);
    void ReloadAllShaders();

    // Presets
    std::shared_ptr<Effect> CreateBasicEffect();
    std::shared_ptr<Effect> CreateMultiTextureEffect();
    std::shared_ptr<Effect> CreateLavaEffect();
    std::shared_ptr<Effect> CreateMetalEffect();
    std::shared_ptr<Effect> CreateWaterEffect();

    // Global shader parameters
    void SetGlobalMatrix(const std::string& name, const D3DXMATRIX& matrix);
    void SetGlobalVector(const std::string& name, const D3DXVECTOR4& vector);
    void SetGlobalFloat(const std::string& name, float value);
    void SetGlobalTexture(const std::string& name, IDirect3DTexture9* texture);

    // Statistics
    size_t GetEffectCount() const { return m_effects.size(); }
    size_t GetTotalMemoryUsage() const;
    std::vector<ShaderInfo> GetLoadedShaders() const;

    // Error handling
    const std::string& GetLastError() const { return m_lastError; }
    void ClearError() { m_lastError.clear(); }

private:
    std::string LoadShaderSource(const std::string& filename);
    std::string GetShaderKey(const std::string& filename, const std::vector<ShaderMacro>& macros);
    D3DXMACRO* ConvertMacros(const std::vector<ShaderMacro>& macros, std::vector<D3DXMACRO>& d3dMacros);
    void ProcessIncludeDirectives(std::string& source, const std::string& baseDir);

    IDirect3DDevice9* m_device;
    std::unordered_map<std::string, std::shared_ptr<Effect>> m_effects;
    std::unordered_map<std::string, IDirect3DVertexShader9*> m_vertexShaders;
    std::unordered_map<std::string, IDirect3DPixelShader9*> m_pixelShaders;

    // Hot reloading
    bool m_hotReloadEnabled;
    std::unordered_map<std::string, FILETIME> m_fileTimestamps;

    // Global parameters
    std::unordered_map<std::string, D3DXMATRIX> m_globalMatrices;
    std::unordered_map<std::string, D3DXVECTOR4> m_globalVectors;
    std::unordered_map<std::string, float> m_globalFloats;
    std::unordered_map<std::string, IDirect3DTexture9*> m_globalTextures;

    // Error handling
    std::string m_lastError;
};
