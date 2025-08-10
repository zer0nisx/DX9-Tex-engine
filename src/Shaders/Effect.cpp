#include "Effect.h"
#include <iostream>

// Helper function to convert string to wide string
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

Effect::Effect()
    : m_device(nullptr)
    , m_effect(nullptr)
    , m_currentTechnique(nullptr)
    , m_isBegun(false)
    , m_currentPass(0)
    , m_numPasses(0)
    , m_worldMatrix(nullptr)
    , m_viewMatrix(nullptr)
    , m_projMatrix(nullptr)
    , m_worldViewProj(nullptr)
    , m_time(nullptr)
    , m_cameraPos(nullptr)
    , m_lightDir(nullptr)
    , m_lightColor(nullptr)
{
}

Effect::~Effect()
{
    Release();
}

bool Effect::LoadFromFile(IDirect3DDevice9* device, const std::string& filename)
{
    if (!device)
    {
        SetError("Invalid device pointer");
        return false;
    }

    m_device = device;
    m_filename = filename;

    // Cargar efecto desde archivo
    ID3DXBuffer* errors = nullptr;
    std::wstring wFilename = StringToWString(filename);
    HRESULT hr = D3DXCreateEffectFromFileW(
        device,
        wFilename.c_str(),
        nullptr,        // macros
        nullptr,        // include
        0,              // flags
        nullptr,        // pool
        &m_effect,
        &errors
    );

    if (FAILED(hr))
    {
        std::string errorMsg = "Failed to load effect from file: " + filename;
        if (errors)
        {
            errorMsg += "\nD3DX Error: ";
            errorMsg += static_cast<char*>(errors->GetBufferPointer());
            errors->Release();
        }
        SetError(errorMsg);
        return false;
    }

    if (errors)
    {
        errors->Release();
    }

    CacheParameters();
    std::cout << "Loaded effect: " << filename << std::endl;
    return true;
}

bool Effect::LoadFromMemory(IDirect3DDevice9* device, const void* data, size_t size)
{
    if (!device || !data)
    {
        SetError("Invalid parameters");
        return false;
    }

    m_device = device;

    // Cargar efecto desde memoria
    ID3DXBuffer* errors = nullptr;
    HRESULT hr = D3DXCreateEffect(
        device,
        data,
        static_cast<UINT>(size),
        nullptr,        // macros
        nullptr,        // include
        0,              // flags
        nullptr,        // pool
        &m_effect,
        &errors
    );

    if (FAILED(hr))
    {
        std::string errorMsg = "Failed to create effect from memory";
        if (errors)
        {
            errorMsg += "\nD3DX Error: ";
            errorMsg += static_cast<char*>(errors->GetBufferPointer());
            errors->Release();
        }
        SetError(errorMsg);
        return false;
    }

    if (errors)
    {
        errors->Release();
    }

    CacheParameters();
    std::cout << "Created effect from memory" << std::endl;
    return true;
}

void Effect::Release()
{
    if (m_effect)
    {
        m_effect->Release();
        m_effect = nullptr;
    }

    m_device = nullptr;
    m_parameterHandles.clear();
    m_techniqueHandles.clear();
    m_currentTechnique = nullptr;
    m_isBegun = false;
}

bool Effect::SetTechnique(const std::string& name)
{
    if (!m_effect)
        return false;

    D3DXHANDLE handle = GetParameterHandle(name);
    if (!handle)
    {
        handle = m_effect->GetTechniqueByName(name.c_str());
        if (!handle)
        {
            SetError("Technique not found: " + name);
            return false;
        }
        m_techniqueHandles[name] = handle;
    }

    HRESULT hr = m_effect->SetTechnique(handle);
    if (SUCCEEDED(hr))
    {
        m_currentTechnique = handle;
        return true;
    }

    SetError("Failed to set technique: " + name);
    return false;
}

bool Effect::SetMatrix(const std::string& name, const D3DXMATRIX& matrix)
{
    if (!m_effect)
        return false;

    D3DXHANDLE handle = GetParameterHandle(name);
    if (!handle)
        return false;

    HRESULT hr = m_effect->SetMatrix(handle, &matrix);
    return SUCCEEDED(hr);
}

bool Effect::SetVector(const std::string& name, const D3DXVECTOR4& vector)
{
    if (!m_effect)
        return false;

    D3DXHANDLE handle = GetParameterHandle(name);
    if (!handle)
        return false;

    HRESULT hr = m_effect->SetVector(handle, &vector);
    return SUCCEEDED(hr);
}

bool Effect::SetFloat(const std::string& name, float value)
{
    if (!m_effect)
        return false;

    D3DXHANDLE handle = GetParameterHandle(name);
    if (!handle)
        return false;

    HRESULT hr = m_effect->SetFloat(handle, value);
    return SUCCEEDED(hr);
}

bool Effect::SetTexture(const std::string& name, IDirect3DTexture9* texture)
{
    if (!m_effect)
        return false;

    D3DXHANDLE handle = GetParameterHandle(name);
    if (!handle)
        return false;

    HRESULT hr = m_effect->SetTexture(handle, texture);
    return SUCCEEDED(hr);
}

bool Effect::Begin(UINT* passes)
{
    if (!m_effect || m_isBegun)
        return false;

    HRESULT hr = m_effect->Begin(passes, 0);
    if (SUCCEEDED(hr))
    {
        m_isBegun = true;
        m_numPasses = *passes;
        m_currentPass = 0;
        return true;
    }

    return false;
}

bool Effect::BeginPass(UINT pass)
{
    if (!m_effect || !m_isBegun)
        return false;

    HRESULT hr = m_effect->BeginPass(pass);
    if (SUCCEEDED(hr))
    {
        m_currentPass = pass;
        return true;
    }

    return false;
}

bool Effect::EndPass()
{
    if (!m_effect || !m_isBegun)
        return false;

    HRESULT hr = m_effect->EndPass();
    return SUCCEEDED(hr);
}

bool Effect::End()
{
    if (!m_effect || !m_isBegun)
        return false;

    HRESULT hr = m_effect->End();
    if (SUCCEEDED(hr))
    {
        m_isBegun = false;
        return true;
    }

    return false;
}

bool Effect::CommitChanges()
{
    if (!m_effect)
        return false;

    HRESULT hr = m_effect->CommitChanges();
    return SUCCEEDED(hr);
}

void Effect::SetupAutomaticParameters(const D3DXMATRIX& world, const D3DXMATRIX& view,
                                     const D3DXMATRIX& projection, float time)
{
    if (!m_effect)
        return;

    // Configurar matrices comunes
    SetMatrix("WorldMatrix", world);
    SetMatrix("ViewMatrix", view);
    SetMatrix("ProjectionMatrix", projection);

    // Calcular y establecer WorldViewProjection
    D3DXMATRIX worldViewProj = world * view * projection;
    SetMatrix("WorldViewProj", worldViewProj);

    // Tiempo para animaciones
    SetFloat("Time", time);
}

void Effect::BindCameraParameters(const D3DXVECTOR3& position, const D3DXVECTOR3& direction)
{
    SetVector("CameraPosition", D3DXVECTOR4(position.x, position.y, position.z, 1.0f));
    SetVector("CameraDirection", D3DXVECTOR4(direction.x, direction.y, direction.z, 0.0f));
}

void Effect::BindLightParameters(const D3DXVECTOR3& direction, const D3DXVECTOR4& color)
{
    SetVector("LightDirection", D3DXVECTOR4(direction.x, direction.y, direction.z, 0.0f));
    SetVector("LightColor", color);
}

bool Effect::Validate() const
{
    if (!m_effect)
        return false;

    HRESULT hr = m_effect->ValidateTechnique(m_currentTechnique);
    return SUCCEEDED(hr);
}

D3DXHANDLE Effect::GetParameterHandle(const std::string& name) const
{
    if (!m_effect)
        return nullptr;

    // Verificar cache
    auto it = m_parameterHandles.find(name);
    if (it != m_parameterHandles.end())
    {
        return it->second;
    }

    // Obtener handle y cachear
    D3DXHANDLE handle = m_effect->GetParameterByName(nullptr, name.c_str());
    if (handle)
    {
        const_cast<std::unordered_map<std::string, D3DXHANDLE>&>(m_parameterHandles)[name] = handle;
    }

    return handle;
}

void Effect::CacheParameters()
{
    if (!m_effect)
        return;

    // Cachear parámetros comunes
    m_worldMatrix = GetParameterHandle("WorldMatrix");
    m_viewMatrix = GetParameterHandle("ViewMatrix");
    m_projMatrix = GetParameterHandle("ProjectionMatrix");
    m_worldViewProj = GetParameterHandle("WorldViewProj");
    m_time = GetParameterHandle("Time");
    m_cameraPos = GetParameterHandle("CameraPosition");
    m_lightDir = GetParameterHandle("LightDirection");
    m_lightColor = GetParameterHandle("LightColor");
}
