#pragma once

#include <windows.h>
#include <memory>
#include <string>

// Forward declarations
class Window;
class Renderer;
class Timer;
class TextureManager;
class ShaderManager;
class Camera;
class Mesh;

class Engine {
public:
    Engine();
    ~Engine();

    // Core functions
    bool Initialize(int width = 1024, int height = 768, const std::string& title = "DX9 Engine");
    void Run();
    void Shutdown();

    // Getters
    Window* GetWindow() const { return m_window.get(); }
    Renderer* GetRenderer() const { return m_renderer.get(); }
    Timer* GetTimer() const { return m_timer.get(); }
    TextureManager* GetTextureManager() const { return m_textureManager.get(); }
    ShaderManager* GetShaderManager() const { return m_shaderManager.get(); }
    Camera* GetCamera() const { return m_camera.get(); }

    // Singleton access
    static Engine* GetInstance() { return s_instance; }

private:
    void Update(float deltaTime);
    void Render();
    void HandleInput(float deltaTime);
    void SwitchMaterial(int materialIndex);
    void CreateDemoMaterials();

    // Core systems
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Timer> m_timer;
    std::unique_ptr<TextureManager> m_textureManager;
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Mesh> m_cube;

    // Engine state
    bool m_isRunning;
    bool m_isInitialized;

    // Singleton
    static Engine* s_instance;
};
