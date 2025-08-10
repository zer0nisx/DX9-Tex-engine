#include "Core/Engine.h"
#include <iostream>
#include <memory>

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Suppress unused parameter warnings
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    // Redirigir salida de consola para debugging
#ifdef _DEBUG
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    std::cout << "=== DirectX 9 Engine - Advanced Texture System ===" << std::endl;
    std::cout << "Initializing engine..." << std::endl;
#endif

    try
    {
        // Crear instancia del motor
        auto engine = std::make_unique<Engine>();

        // Inicializar motor con resolución y título
        if (!engine->Initialize(1024, 768, "DX9 Engine - Advanced Textures"))
        {
            std::cerr << "Failed to initialize engine!" << std::endl;
            return -1;
        }

        std::cout << "Engine initialized successfully!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Mouse - Look around" << std::endl;
        std::cout << "  1-5 - Switch materials" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;

        // Ejecutar loop principal
        engine->Run();

        std::cout << "Engine shutting down..." << std::endl;

        // Cleanup automático por RAII
        engine->Shutdown();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught!" << std::endl;
        return -1;
    }

#ifdef _DEBUG
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
    FreeConsole();
#endif

    return 0;
}
