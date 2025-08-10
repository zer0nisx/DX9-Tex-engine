#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class Texture;
class Effect;

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

enum class BlendMode {
    REPLACE = 0,
    MULTIPLY,
    ADD,
    SUBTRACT,
    MODULATE,
    MODULATE2X,
    MODULATE4X,
    ADD_SIGNED,
    ADD_SIGNED2X,
    ALPHA_BLEND,
    LERP,
    SUBTRACT_REV
};

struct TextureLayer {
    std::shared_ptr<Texture> texture;
    BlendMode colorBlend = BlendMode::MODULATE;
    BlendMode alphaBlend = BlendMode::MODULATE;
    D3DCOLOR colorArg = D3DTA_CURRENT;
    D3DCOLOR alphaArg = D3DTA_CURRENT;
    float uvOffsetU = 0.0f;
    float uvOffsetV = 0.0f;
    float uvScaleU = 1.0f;
    float uvScaleV = 1.0f;
    float uvRotation = 0.0f;
    int uvChannel = 0;
    bool enabled = true;
};

struct MaterialProperties {
    D3DCOLORVALUE diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    D3DCOLORVALUE ambient = {0.2f, 0.2f, 0.2f, 1.0f};
    D3DCOLORVALUE specular = {0.0f, 0.0f, 0.0f, 1.0f};
    D3DCOLORVALUE emissive = {0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 0.0f;
    float metallic = 0.0f;
    float roughness = 0.5f;
    float normalStrength = 1.0f;
    float parallaxScale = 0.0f;
    float emissionIntensity = 1.0f;
};

class Material {
public:
    Material(const std::string& name = "");
    ~Material();

    // Texture management
    void AddTexture(TextureType type, std::shared_ptr<Texture> texture, int layer = 0);
    void AddTexture(TextureType type, const std::string& filename, int layer = 0);
    void RemoveTexture(TextureType type, int layer = 0);
    void RemoveAllTextures(TextureType type);
    std::shared_ptr<Texture> GetTexture(TextureType type, int layer = 0) const;

    // Layer management
    void SetTextureLayer(int stage, const TextureLayer& layer);
    TextureLayer& GetTextureLayer(int stage);
    const TextureLayer& GetTextureLayer(int stage) const;
    void EnableLayer(int stage, bool enable);
    bool IsLayerEnabled(int stage) const;
    int GetLayerCount() const;

    // Blending
    void SetBlendMode(int stage, BlendMode colorBlend, BlendMode alphaBlend = BlendMode::MODULATE);
    void SetBlendArgs(int stage, D3DCOLOR colorArg, D3DCOLOR alphaArg);

    // UV manipulation
    void SetUVTransform(int stage, float offsetU, float offsetV, float scaleU, float scaleV, float rotation = 0.0f);
    void SetUVOffset(int stage, float u, float v);
    void SetUVScale(int stage, float u, float v);
    void SetUVRotation(int stage, float rotation);
    void SetUVChannel(int stage, int channel);

    // Animation
    void UpdateAnimation(float deltaTime);
    void SetScrollSpeed(int stage, float speedU, float speedV);
    void SetPulseSettings(int stage, float frequency, float amplitude);

    // Material properties
    void SetProperties(const MaterialProperties& props) { m_properties = props; }
    const MaterialProperties& GetProperties() const { return m_properties; }
    void SetDiffuseColor(const D3DCOLORVALUE& color) { m_properties.diffuse = color; }
    void SetSpecularColor(const D3DCOLORVALUE& color) { m_properties.specular = color; }
    void SetEmissiveColor(const D3DCOLORVALUE& color) { m_properties.emissive = color; }
    void SetShininess(float shininess) { m_properties.shininess = shininess; }
    void SetMetallic(float metallic) { m_properties.metallic = metallic; }
    void SetRoughness(float roughness) { m_properties.roughness = roughness; }

    // Shader integration
    void SetEffect(std::shared_ptr<Effect> effect);
    std::shared_ptr<Effect> GetEffect() const { return m_effect; }
    void SetShaderParameter(const std::string& name, float value);
    void SetShaderParameter(const std::string& name, const D3DXVECTOR4& value);
    void SetShaderParameter(const std::string& name, const D3DXMATRIX& value);

    // Application
    void Apply(IDirect3DDevice9* device) const;
    void ApplyTextures(IDirect3DDevice9* device) const;
    void ApplyMaterialProperties(IDirect3DDevice9* device) const;
    void ApplyTextureStates(IDirect3DDevice9* device) const;

    // State management
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    bool IsTransparent() const;
    bool HasAlpha() const;

    // Presets
    static std::shared_ptr<Material> CreateDefaultMaterial();
    static std::shared_ptr<Material> CreateMetalMaterial(float metallic = 1.0f, float roughness = 0.1f);
    static std::shared_ptr<Material> CreateGlassMaterial(float transparency = 0.8f);
    static std::shared_ptr<Material> CreateLavaMaterial();
    static std::shared_ptr<Material> CreateWaterMaterial();

private:
    D3DTEXTUREOP ConvertBlendMode(BlendMode mode) const;
    void ApplyLayer(IDirect3DDevice9* device, int stage, const TextureLayer& layer) const;
    void CalculateUVMatrix(const TextureLayer& layer, D3DXMATRIX& matrix) const;

    std::string m_name;
    std::vector<TextureLayer> m_layers;
    MaterialProperties m_properties;
    std::shared_ptr<Effect> m_effect;

    // Animation data
    struct LayerAnimation {
        float scrollSpeedU = 0.0f;
        float scrollSpeedV = 0.0f;
        float pulseFrequency = 0.0f;
        float pulseAmplitude = 0.0f;
        float currentTime = 0.0f;
    };
    std::vector<LayerAnimation> m_layerAnimations;

    // Shader parameters
    std::unordered_map<std::string, float> m_floatParams;
    std::unordered_map<std::string, D3DXVECTOR4> m_vectorParams;
    std::unordered_map<std::string, D3DXMATRIX> m_matrixParams;

    static const int MAX_TEXTURE_STAGES = 8;
};
