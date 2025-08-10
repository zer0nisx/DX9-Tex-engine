#include "Material.h"
#include "Texture.h"
#include "../Shaders/Effect.h"
#include <iostream>
#include <algorithm>

Material::Material(const std::string& name)
    : m_name(name)
    , m_layers(MAX_TEXTURE_STAGES)
    , m_layerAnimations(MAX_TEXTURE_STAGES)
{
    // Configurar propiedades por defecto
    m_properties.diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    m_properties.ambient = {0.2f, 0.2f, 0.2f, 1.0f};
    m_properties.specular = {0.0f, 0.0f, 0.0f, 1.0f};
    m_properties.emissive = {0.0f, 0.0f, 0.0f, 1.0f};
    m_properties.shininess = 32.0f;
    m_properties.metallic = 0.0f;
    m_properties.roughness = 0.5f;
    m_properties.normalStrength = 1.0f;
    m_properties.parallaxScale = 0.0f;
    m_properties.emissionIntensity = 1.0f;

    // Inicializar capas
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        m_layers[i].enabled = (i == 0); // Solo primera capa habilitada por defecto
        m_layers[i].colorBlend = BlendMode::MODULATE;
        m_layers[i].alphaBlend = BlendMode::MODULATE;
        m_layers[i].uvScaleU = 1.0f;
        m_layers[i].uvScaleV = 1.0f;
    }
}

Material::~Material() = default;

void Material::AddTexture(TextureType type, std::shared_ptr<Texture> texture, int layer)
{
    if (layer >= 0 && layer < MAX_TEXTURE_STAGES && texture)
    {
        m_layers[layer].texture = texture;
        m_layers[layer].enabled = true;

        std::cout << "Added texture to material '" << m_name << "' at layer " << layer << std::endl;
    }
}

void Material::AddTexture(TextureType type, const std::string& filename, int layer)
{
    // Esta función requeriría acceso al TextureManager
    // Por ahora, simplemente log
    std::cout << "Material::AddTexture by filename not implemented yet" << std::endl;
}

std::shared_ptr<Texture> Material::GetTexture(TextureType type, int layer) const
{
    if (layer >= 0 && layer < MAX_TEXTURE_STAGES)
    {
        return m_layers[layer].texture;
    }
    return nullptr;
}

void Material::SetTextureLayer(int stage, const TextureLayer& layer)
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        m_layers[stage] = layer;
    }
}

TextureLayer& Material::GetTextureLayer(int stage)
{
    static TextureLayer dummy;
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        return m_layers[stage];
    }
    return dummy;
}

const TextureLayer& Material::GetTextureLayer(int stage) const
{
    static TextureLayer dummy;
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        return m_layers[stage];
    }
    return dummy;
}

void Material::EnableLayer(int stage, bool enable)
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        m_layers[stage].enabled = enable;
    }
}

bool Material::IsLayerEnabled(int stage) const
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        return m_layers[stage].enabled;
    }
    return false;
}

int Material::GetLayerCount() const
{
    int count = 0;
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        if (m_layers[i].enabled && m_layers[i].texture)
        {
            count++;
        }
    }
    return count;
}

void Material::SetBlendMode(int stage, BlendMode colorBlend, BlendMode alphaBlend)
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        m_layers[stage].colorBlend = colorBlend;
        m_layers[stage].alphaBlend = alphaBlend;
    }
}

void Material::SetUVTransform(int stage, float offsetU, float offsetV, float scaleU, float scaleV, float rotation)
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        m_layers[stage].uvOffsetU = offsetU;
        m_layers[stage].uvOffsetV = offsetV;
        m_layers[stage].uvScaleU = scaleU;
        m_layers[stage].uvScaleV = scaleV;
        m_layers[stage].uvRotation = rotation;
    }
}

void Material::UpdateAnimation(float deltaTime)
{
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        if (m_layers[i].enabled && m_layers[i].texture)
        {
            m_layerAnimations[i].currentTime += deltaTime;

            // Aplicar scroll
            if (m_layerAnimations[i].scrollSpeedU != 0.0f || m_layerAnimations[i].scrollSpeedV != 0.0f)
            {
                m_layers[i].uvOffsetU += m_layerAnimations[i].scrollSpeedU * deltaTime;
                m_layers[i].uvOffsetV += m_layerAnimations[i].scrollSpeedV * deltaTime;

                // Normalizar offsets para evitar overflow
                m_layers[i].uvOffsetU = fmodf(m_layers[i].uvOffsetU, 1.0f);
                m_layers[i].uvOffsetV = fmodf(m_layers[i].uvOffsetV, 1.0f);
            }

            // Aplicar pulsación
            if (m_layerAnimations[i].pulseFrequency > 0.0f)
            {
                float pulse = sinf(m_layerAnimations[i].currentTime * m_layerAnimations[i].pulseFrequency * 6.28f);
                // El efecto de pulsación se podría aplicar a la intensidad o escala
            }
        }
    }
}

void Material::SetScrollSpeed(int stage, float speedU, float speedV)
{
    if (stage >= 0 && stage < MAX_TEXTURE_STAGES)
    {
        m_layerAnimations[stage].scrollSpeedU = speedU;
        m_layerAnimations[stage].scrollSpeedV = speedV;
    }
}

void Material::SetEffect(std::shared_ptr<Effect> effect)
{
    m_effect = effect;
}

void Material::Apply(IDirect3DDevice9* device) const
{
    if (!device)
        return;

    // Aplicar propiedades del material
    ApplyMaterialProperties(device);

    // Aplicar texturas
    ApplyTextures(device);

    // Aplicar estados de textura
    ApplyTextureStates(device);

    // Aplicar efecto si existe
    if (m_effect)
    {
        // El efecto se encargará de su propia aplicación
        // Por ahora simplemente aplicamos las texturas manualmente
    }
}

void Material::ApplyTextures(IDirect3DDevice9* device) const
{
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        if (m_layers[i].enabled && m_layers[i].texture)
        {
            m_layers[i].texture->Bind(i);
        }
        else
        {
            device->SetTexture(i, nullptr);
        }
    }
}

void Material::ApplyMaterialProperties(IDirect3DDevice9* device) const
{
    D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(material));

    material.Diffuse = m_properties.diffuse;
    material.Ambient = m_properties.ambient;
    material.Specular = m_properties.specular;
    material.Emissive = m_properties.emissive;
    material.Power = m_properties.shininess;

    device->SetMaterial(&material);
}

void Material::ApplyTextureStates(IDirect3DDevice9* device) const
{
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        if (m_layers[i].enabled && m_layers[i].texture)
        {
            ApplyLayer(device, i, m_layers[i]);
        }
    }
}

void Material::ApplyLayer(IDirect3DDevice9* device, int stage, const TextureLayer& layer) const
{
    // Configurar operaciones de blending
    D3DTEXTUREOP colorOp = ConvertBlendMode(layer.colorBlend);
    D3DTEXTUREOP alphaOp = ConvertBlendMode(layer.alphaBlend);

    device->SetTextureStageState(stage, D3DTSS_COLOROP, colorOp);
    device->SetTextureStageState(stage, D3DTSS_ALPHAOP, alphaOp);

    device->SetTextureStageState(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(stage, D3DTSS_COLORARG2, stage == 0 ? D3DTA_DIFFUSE : D3DTA_CURRENT);
    device->SetTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(stage, D3DTSS_ALPHAARG2, stage == 0 ? D3DTA_DIFFUSE : D3DTA_CURRENT);

    // UV Transform - esto requeriría matrices de transformación
    // Por simplicidad, lo omitimos por ahora
}

D3DTEXTUREOP Material::ConvertBlendMode(BlendMode mode) const
{
    switch (mode)
    {
    case BlendMode::REPLACE:
        return D3DTOP_SELECTARG1;
    case BlendMode::MULTIPLY:
    case BlendMode::MODULATE:
        return D3DTOP_MODULATE;
    case BlendMode::ADD:
        return D3DTOP_ADD;
    case BlendMode::SUBTRACT:
        return D3DTOP_SUBTRACT;
    case BlendMode::MODULATE2X:
        return D3DTOP_MODULATE2X;
    case BlendMode::MODULATE4X:
        return D3DTOP_MODULATE4X;
    case BlendMode::ADD_SIGNED:
        return D3DTOP_ADDSIGNED;
    case BlendMode::ADD_SIGNED2X:
        return D3DTOP_ADDSIGNED2X;
    case BlendMode::ALPHA_BLEND:
        return D3DTOP_BLENDTEXTUREALPHA;
    case BlendMode::LERP:
        return D3DTOP_LERP;
    case BlendMode::SUBTRACT_REV:
        return D3DTOP_SUBTRACT;
    default:
        return D3DTOP_MODULATE;
    }
}

bool Material::IsTransparent() const
{
    return m_properties.diffuse.a < 1.0f;
}

bool Material::HasAlpha() const
{
    for (int i = 0; i < MAX_TEXTURE_STAGES; i++)
    {
        if (m_layers[i].enabled && m_layers[i].texture)
        {
            // Verificar si la textura tiene canal alpha
            // Por simplicidad, asumimos que sí
            return true;
        }
    }
    return false;
}

// Factory methods
std::shared_ptr<Material> Material::CreateDefaultMaterial()
{
    auto material = std::make_shared<Material>("Default");
    material->SetDiffuseColor({1.0f, 1.0f, 1.0f, 1.0f});
    return material;
}

std::shared_ptr<Material> Material::CreateMetalMaterial(float metallic, float roughness)
{
    auto material = std::make_shared<Material>("Metal");
    material->SetDiffuseColor({0.7f, 0.7f, 0.8f, 1.0f});
    material->SetSpecularColor({1.0f, 1.0f, 1.0f, 1.0f});
    material->SetShininess(128.0f);
    material->SetMetallic(metallic);
    material->SetRoughness(roughness);
    return material;
}

std::shared_ptr<Material> Material::CreateLavaMaterial()
{
    auto material = std::make_shared<Material>("Lava");
    material->SetDiffuseColor({1.0f, 0.3f, 0.1f, 1.0f});
    material->SetEmissiveColor({1.0f, 0.4f, 0.0f, 1.0f});
    material->SetScrollSpeed(0, 0.1f, 0.05f);
    return material;
}

std::shared_ptr<Material> Material::CreateWaterMaterial()
{
    auto material = std::make_shared<Material>("Water");
    material->SetDiffuseColor({0.0f, 0.3f, 0.8f, 0.7f});
    material->SetSpecularColor({1.0f, 1.0f, 1.0f, 1.0f});
    material->SetShininess(64.0f);
    return material;
}
