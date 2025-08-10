#include "Engine.h"
#include "Window.h"
#include "Timer.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Mesh.h"
#include "../Textures/TextureManager.h"
#include "../Textures/Material.h"
#include "../Textures/TextureEffects.h"
#include "../Shaders/ShaderManager.h"

#include <iostream>
#include <memory>

// Singleton instance
Engine* Engine::s_instance = nullptr;

Engine::Engine()
    : m_isRunning(false)
    , m_isInitialized(false)
{
    s_instance = this;
}

Engine::~Engine()
{
    if (m_isInitialized)
    {
        Shutdown();
    }
    s_instance = nullptr;
}

bool Engine::Initialize(int width, int height, const std::string& title)
{
    if (m_isInitialized)
        return true;

    std::cout << "Initializing Engine..." << std::endl;

    // Crear ventana
    m_window = std::make_unique<Window>();
    if (!m_window->Create(width, height, title))
    {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    // Crear timer
    m_timer = std::make_unique<Timer>();
    m_timer->Start();

    // Crear renderer
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Initialize(m_window->GetHandle(), width, height, false))
    {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }

    // Crear texture manager
    m_textureManager = std::make_unique<TextureManager>();
    if (!m_textureManager->Initialize(m_renderer->GetDevice()))
    {
        std::cerr << "Failed to initialize texture manager!" << std::endl;
        return false;
    }

    // Crear shader manager
    m_shaderManager = std::make_unique<ShaderManager>();
    if (!m_shaderManager->Initialize(m_renderer->GetDevice()))
    {
        std::cerr << "Failed to initialize shader manager!" << std::endl;
        return false;
    }

    // Crear cámara
    m_camera = std::make_unique<Camera>();
    m_camera->Initialize(D3DX_PI / 4.0f, (float)width / height, 0.1f, 100.0f);
    m_camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -5.0f));
    m_camera->SetTarget(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

    // Crear cubo
    m_cube = std::make_unique<Mesh>();
    if (!m_cube->CreateCube(m_renderer->GetDevice(), 2.0f))
    {
        std::cerr << "Failed to create cube mesh!" << std::endl;
        return false;
    }

    // Crear materiales de demostración
    CreateDemoMaterials();

    std::cout << "Engine initialized successfully!" << std::endl;
    m_isInitialized = true;
    return true;
}

void Engine::CreateDemoMaterials()
{
    auto device = m_renderer->GetDevice();

    // Material 1: Lava animada
    auto lavaMaterial = Material::CreateLavaMaterial();
    auto lavaTexture = m_textureManager->CreateNoiseTexture("lava_noise", 256, 256, 4.0f, 4);
    lavaMaterial->AddTexture(TextureType::DIFFUSE, lavaTexture);
    lavaMaterial->SetEffect(m_shaderManager->CreateLavaEffect());
    m_cube->SetMaterial(lavaMaterial);

    std::cout << "Created demo materials!" << std::endl;
}

void Engine::Run()
{
    if (!m_isInitialized)
        return;

    m_isRunning = true;
    m_timer->Reset();

    std::cout << "Starting main loop..." << std::endl;

    while (m_isRunning && !m_window->ShouldClose())
    {
        // Procesar mensajes de ventana
        m_window->ProcessMessages();

        // Actualizar timer
        m_timer->Update();
        float deltaTime = m_timer->GetDeltaTime();

        // Verificar si el dispositivo se perdió
        if (m_renderer->CheckDeviceLost())
        {
            m_renderer->HandleDeviceLost();
            continue;
        }

        // Actualizar motor
        Update(deltaTime);

        // Renderizar frame
        Render();

        // Controlar framerate si es necesario
        if (m_timer->GetFPS() > 1000.0f)
        {
            Sleep(1);
        }
    }

    std::cout << "Main loop ended." << std::endl;
}

void Engine::Update(float deltaTime)
{
    // Procesar input
    HandleInput(deltaTime);

    // Actualizar cámara
    m_camera->Update();

    // Actualizar efectos de texturas
    g_effectManager.Update(deltaTime);

    // Actualizar materiales animados
    if (m_cube && m_cube->GetMaterial())
    {
        m_cube->GetMaterial()->UpdateAnimation(deltaTime);
    }
}

void Engine::HandleInput(float deltaTime)
{
    // Input básico de teclado
    bool forward = GetAsyncKeyState('W') & 0x8000;
    bool backward = GetAsyncKeyState('S') & 0x8000;
    bool left = GetAsyncKeyState('A') & 0x8000;
    bool right = GetAsyncKeyState('D') & 0x8000;
    bool up = GetAsyncKeyState(VK_SPACE) & 0x8000;
    bool down = GetAsyncKeyState(VK_LCONTROL) & 0x8000;

    // Mover cámara
    m_camera->HandleKeyboard(forward, backward, left, right, up, down, deltaTime, 5.0f);

    // Cambiar materiales con teclas numéricas
    static bool keyPressed[6] = {false};

    for (int i = 1; i <= 5; i++)
    {
        bool currentlyPressed = GetAsyncKeyState('0' + i) & 0x8000;
        if (currentlyPressed && !keyPressed[i])
        {
            SwitchMaterial(i);
        }
        keyPressed[i] = currentlyPressed;
    }

    // ESC para salir
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        m_isRunning = false;
    }
}

void Engine::SwitchMaterial(int materialIndex)
{
    if (!m_cube)
        return;

    // Device pointer for material creation (unused for now)
    // auto device = m_renderer->GetDevice();
    std::shared_ptr<Material> newMaterial;

    switch (materialIndex)
    {
    case 1:
        // Material de lava
        newMaterial = Material::CreateLavaMaterial();
        newMaterial->SetEffect(m_shaderManager->CreateLavaEffect());
        std::cout << "Switched to Lava material" << std::endl;
        break;

    case 2:
        // Material metálico
        newMaterial = Material::CreateMetalMaterial(0.9f, 0.1f);
        newMaterial->SetEffect(m_shaderManager->CreateMetalEffect());
        std::cout << "Switched to Metal material" << std::endl;
        break;

    case 3:
        // Material de agua
        newMaterial = Material::CreateWaterMaterial();
        newMaterial->SetEffect(m_shaderManager->CreateWaterEffect());
        std::cout << "Switched to Water material" << std::endl;
        break;

    case 4:
        {
            // Material multi-texture
            newMaterial = std::make_shared<Material>("MultiTexture");
            auto noiseTexture1 = m_textureManager->CreateNoiseTexture("noise1", 256, 256, 2.0f, 4);
            auto noiseTexture2 = m_textureManager->CreateNoiseTexture("noise2", 256, 256, 4.0f, 3);
            newMaterial->AddTexture(TextureType::DIFFUSE, noiseTexture1, 0);
            newMaterial->AddTexture(TextureType::DIFFUSE, noiseTexture2, 1);
            newMaterial->SetEffect(m_shaderManager->CreateMultiTextureEffect());
            std::cout << "Switched to Multi-Texture material" << std::endl;
            break;
        }

    case 5:
        {
            // Material básico
            newMaterial = Material::CreateDefaultMaterial();
            auto checkerTexture = m_textureManager->CreateProceduralTexture("checker", 256, 256);
            newMaterial->AddTexture(TextureType::DIFFUSE, checkerTexture);
            newMaterial->SetEffect(m_shaderManager->CreateBasicEffect());
            std::cout << "Switched to Basic material" << std::endl;
            break;
        }

    default:
        return;
    }

    if (newMaterial)
    {
        m_cube->SetMaterial(newMaterial);
    }
}

void Engine::Render()
{
    // Comenzar frame
    m_renderer->BeginFrame();
    m_renderer->Clear(D3DCOLOR_XRGB(50, 50, 100));

    // Configurar matrices
    m_renderer->SetupMatrices(m_camera.get());

    // Renderizar cubo
    if (m_cube && m_cube->GetMaterial())
    {
        D3DXMATRIX worldMatrix;
        D3DXMatrixIdentity(&worldMatrix);

        // Rotar cubo lentamente
        static float rotation = 0.0f;
        rotation += m_timer->GetDeltaTime() * 0.5f;
        D3DXMatrixRotationYawPitchRoll(&worldMatrix, rotation, rotation * 0.7f, 0.0f);

        m_renderer->RenderMesh(m_cube.get(), m_cube->GetMaterial().get(), worldMatrix);
    }

    // Finalizar frame
    m_renderer->EndFrame();
    m_renderer->Present();
}

void Engine::Shutdown()
{
    if (!m_isInitialized)
        return;

    std::cout << "Shutting down engine..." << std::endl;

    m_isRunning = false;

    // Liberar recursos en orden inverso
    m_cube.reset();
    m_camera.reset();
    m_shaderManager.reset();
    m_textureManager.reset();
    m_renderer.reset();
    m_timer.reset();
    m_window.reset();

    m_isInitialized = false;
    std::cout << "Engine shutdown complete." << std::endl;
}
