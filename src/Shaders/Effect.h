#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <unordered_map>
#include <vector>

class Effect {
public:
    Effect();
    ~Effect();

    // Initialization
    bool LoadFromFile(IDirect3DDevice9* device, const std::string& filename);
    bool LoadFromMemory(IDirect3DDevice9* device, const void* data, size_t size);
    void Release();

    // Technique management
    bool SetTechnique(const std::string& name);
    bool SetTechnique(D3DXHANDLE handle);
    std::string GetCurrentTechnique() const;
    std::vector<std::string> GetTechniqueList() const;
    bool ValidateTechnique(const std::string& name) const;

    // Parameter setting
    bool SetMatrix(const std::string& name, const D3DXMATRIX& matrix);
    bool SetMatrixArray(const std::string& name, const D3DXMATRIX* matrices, int count);
    bool SetVector(const std::string& name, const D3DXVECTOR4& vector);
    bool SetVectorArray(const std::string& name, const D3DXVECTOR4* vectors, int count);
    bool SetFloat(const std::string& name, float value);
    bool SetFloatArray(const std::string& name, const float* values, int count);
    bool SetInt(const std::string& name, int value);
    bool SetBool(const std::string& name, bool value);
    bool SetTexture(const std::string& name, IDirect3DTexture9* texture);
    bool SetCubeTexture(const std::string& name, IDirect3DCubeTexture9* texture);

    // Parameter getting
    bool GetMatrix(const std::string& name, D3DXMATRIX& matrix) const;
    bool GetVector(const std::string& name, D3DXVECTOR4& vector) const;
    bool GetFloat(const std::string& name, float& value) const;
    bool GetInt(const std::string& name, int& value) const;
    bool GetBool(const std::string& name, bool& value) const;

    // Rendering
    bool Begin(UINT* passes);
    bool BeginPass(UINT pass);
    bool EndPass();
    bool End();
    bool CommitChanges();

    // Parameter info
    struct ParameterInfo {
        std::string name;
        D3DXPARAMETER_CLASS paramClass;
        D3DXPARAMETER_TYPE paramType;
        UINT rows;
        UINT columns;
        UINT elements;
        std::string semantic;
        bool isUsed;
    };

    std::vector<ParameterInfo> GetParameterList() const;
    bool HasParameter(const std::string& name) const;
    ParameterInfo GetParameterInfo(const std::string& name) const;

    // Automatic parameter binding
    void SetupAutomaticParameters(const D3DXMATRIX& world, const D3DXMATRIX& view,
                                 const D3DXMATRIX& projection, float time);
    void BindCameraParameters(const D3DXVECTOR3& position, const D3DXVECTOR3& direction);
    void BindLightParameters(const D3DXVECTOR3& direction, const D3DXVECTOR4& color);

    // Preset effects
    void SetupBasicLighting(const D3DXVECTOR3& lightDir, const D3DXVECTOR4& lightColor);
    void SetupMultiTexturing(const std::vector<IDirect3DTexture9*>& textures);
    void SetupAnimatedEffect(float time, float speed = 1.0f);

    // Validation and debugging
    bool Validate() const;
    std::string GetLastError() const { return m_lastError; }
    bool IsValid() const { return m_effect != nullptr; }

    // Direct access
    ID3DXEffect* GetD3DXEffect() const { return m_effect; }
    const std::string& GetFilename() const { return m_filename; }

    // State management
    void SaveState();
    void RestoreState();

private:
    D3DXHANDLE GetParameterHandle(const std::string& name) const;
    void CacheParameters();
    void SetError(const std::string& error) { m_lastError = error; }

    IDirect3DDevice9* m_device;
    ID3DXEffect* m_effect;
    std::string m_filename;

    // Cached handles
    std::unordered_map<std::string, D3DXHANDLE> m_parameterHandles;
    std::unordered_map<std::string, D3DXHANDLE> m_techniqueHandles;

    // Current state
    D3DXHANDLE m_currentTechnique;
    bool m_isBegun;
    UINT m_currentPass;
    UINT m_numPasses;

    // Automatic parameters
    D3DXHANDLE m_worldMatrix;
    D3DXHANDLE m_viewMatrix;
    D3DXHANDLE m_projMatrix;
    D3DXHANDLE m_worldViewProj;
    D3DXHANDLE m_time;
    D3DXHANDLE m_cameraPos;
    D3DXHANDLE m_lightDir;
    D3DXHANDLE m_lightColor;

    // Error handling
    mutable std::string m_lastError;
};
