#include "ShaderManager.h"
#include "Effect.h"
#include <iostream>
#include <fstream>
#include <sstream>

ShaderManager::ShaderManager()
    : m_device(nullptr)
    , m_hotReloadEnabled(false)
{
}

ShaderManager::~ShaderManager()
{
    Shutdown();
}

bool ShaderManager::Initialize(IDirect3DDevice9* device)
{
    if (!device)
    {
        std::cerr << "Invalid device pointer!" << std::endl;
        return false;
    }

    m_device = device;

    std::cout << "ShaderManager initialized successfully!" << std::endl;
    return true;
}

void ShaderManager::Shutdown()
{
    UnloadAllEffects();

    // Liberar vertex shaders
    for (auto& pair : m_vertexShaders)
    {
        if (pair.second)
        {
            pair.second->Release();
        }
    }
    m_vertexShaders.clear();

    // Liberar pixel shaders
    for (auto& pair : m_pixelShaders)
    {
        if (pair.second)
        {
            pair.second->Release();
        }
    }
    m_pixelShaders.clear();

    m_device = nullptr;
}

std::shared_ptr<Effect> ShaderManager::LoadEffect(const std::string& filename, const std::vector<ShaderMacro>& macros)
{
    std::string key = GetShaderKey(filename, macros);

    // Verificar si ya está cargado
    auto it = m_effects.find(key);
    if (it != m_effects.end())
    {
        return it->second;
    }

    // Cargar código fuente
    std::string source = LoadShaderSource(filename);
    if (source.empty())
    {
        std::cerr << "Failed to load shader source: " << filename << std::endl;
        return nullptr;
    }

    // Crear efecto
    auto effect = std::make_shared<Effect>();
    if (!effect->LoadFromMemory(m_device, source.c_str(), source.length()))
    {
        std::cerr << "Failed to compile effect: " << filename << std::endl;
        return nullptr;
    }

    // Almacenar en cache
    m_effects[key] = effect;

    std::cout << "Loaded effect: " << filename << std::endl;
    return effect;
}

std::shared_ptr<Effect> ShaderManager::GetEffect(const std::string& name)
{
    auto it = m_effects.find(name);
    return (it != m_effects.end()) ? it->second : nullptr;
}

void ShaderManager::UnloadEffect(const std::string& name)
{
    auto it = m_effects.find(name);
    if (it != m_effects.end())
    {
        m_effects.erase(it);
        std::cout << "Unloaded effect: " << name << std::endl;
    }
}

void ShaderManager::UnloadAllEffects()
{
    size_t count = m_effects.size();
    m_effects.clear();

    if (count > 0)
    {
        std::cout << "Unloaded " << count << " effects" << std::endl;
    }
}

std::string ShaderManager::LoadShaderSource(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string ShaderManager::GetShaderKey(const std::string& filename, const std::vector<ShaderMacro>& macros)
{
    std::string key = filename;
    for (const auto& macro : macros)
    {
        key += "_" + macro.name + "=" + macro.definition;
    }
    return key;
}

// Factory methods para efectos básicos
std::shared_ptr<Effect> ShaderManager::CreateBasicEffect()
{
    // Por ahora retornamos un efecto vacío
    // En una implementación completa, cargaríamos el shader basic.hlsl
    auto effect = std::make_shared<Effect>();
    std::cout << "Created basic effect (placeholder)" << std::endl;
    return effect;
}

std::shared_ptr<Effect> ShaderManager::CreateMultiTextureEffect()
{
    auto effect = std::make_shared<Effect>();
    std::cout << "Created multi-texture effect (placeholder)" << std::endl;
    return effect;
}

std::shared_ptr<Effect> ShaderManager::CreateLavaEffect()
{
    auto effect = std::make_shared<Effect>();
    std::cout << "Created lava effect (placeholder)" << std::endl;
    return effect;
}

std::shared_ptr<Effect> ShaderManager::CreateMetalEffect()
{
    auto effect = std::make_shared<Effect>();
    std::cout << "Created metal effect (placeholder)" << std::endl;
    return effect;
}

std::shared_ptr<Effect> ShaderManager::CreateWaterEffect()
{
    auto effect = std::make_shared<Effect>();
    std::cout << "Created water effect (placeholder)" << std::endl;
    return effect;
}

size_t ShaderManager::GetTotalMemoryUsage() const
{
    // Cálculo aproximado
    return m_effects.size() * 1024; // Placeholder
}

std::vector<ShaderInfo> ShaderManager::GetLoadedShaders() const
{
    std::vector<ShaderInfo> infos;

    for (const auto& pair : m_effects)
    {
        ShaderInfo info;
        info.filename = pair.first;
        info.isCompiled = true;
        info.memoryUsage = 1024; // Placeholder
        infos.push_back(info);
    }

    return infos;
}
