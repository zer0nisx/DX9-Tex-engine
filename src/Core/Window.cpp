#include "Window.h"
#include "Utils.h"
#include <iostream>
#include <string>

Window::Window()
    : m_hwnd(nullptr)
    , m_hinstance(GetModuleHandle(nullptr))
    , m_className(L"DX9EngineWindowClass")
    , m_width(0)
    , m_height(0)
    , m_shouldClose(false)
    , m_isActive(true)
{
}

Window::~Window()
{
    Destroy();
}

bool Window::Create(int width, int height, const std::string& title)
{
    m_width = width;
    m_height = height;
    m_title = title;

    // Registrar clase de ventana
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // No borrar fondo
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = m_className.c_str();
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassExW(&wc))
    {
        std::cerr << "Failed to register window class!" << std::endl;
        return false;
    }

    // Calcular tamaño de ventana incluyendo bordes
    RECT rect = { 0, 0, width, height };
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect, style, FALSE);

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    // Centrar ventana en la pantalla
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    // Convertir titulo a wide string
    std::wstring wTitle = StringToWString(title);

    // Crear ventana
    m_hwnd = CreateWindowExW(
        0,                          // dwExStyle
        m_className.c_str(),        // lpClassName
        wTitle.c_str(),             // lpWindowName
        style,                      // dwStyle
        x, y,                       // x, y
        windowWidth, windowHeight,  // nWidth, nHeight
        nullptr,                    // hWndParent
        nullptr,                    // hMenu
        m_hinstance,                // hInstance
        this                        // lpParam
    );

    if (!m_hwnd)
    {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    std::cout << "Window created successfully: " << width << "x" << height << std::endl;
    return true;
}

void Window::Destroy()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    // Desregistrar clase
    UnregisterClassW(m_className.c_str(), m_hinstance);
}

void Window::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;

    if (msg == WM_NCCREATE)
    {
        // Obtener puntero this del parámetro de CreateWindow
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }
    else
    {
        // Obtener puntero this almacenado
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->HandleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        m_shouldClose = true;
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_ACTIVATE:
        m_isActive = (LOWORD(wParam) != WA_INACTIVE);
        return 0;

    case WM_SIZE:
        {
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);

            if (newWidth != m_width || newHeight != m_height)
            {
                m_width = newWidth;
                m_height = newHeight;

                // Notificar cambio de tamaño si es necesario
                // Esto se podría usar para redimensionar el back buffer
            }
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            m_shouldClose = true;
        }
        return 0;

    case WM_SYSKEYDOWN:
        // Alt+F4
        if (wParam == VK_F4)
        {
            m_shouldClose = true;
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            // Calcular delta del mouse para cámara
            static int lastX = LOWORD(lParam);
            static int lastY = HIWORD(lParam);
            static bool firstMouse = true;

            int currentX = LOWORD(lParam);
            int currentY = HIWORD(lParam);

            if (!firstMouse)
            {
                int deltaX = currentX - lastX;
                int deltaY = currentY - lastY;

                // Suppress unused variable warnings
                (void)deltaX;
                (void)deltaY;

                // Aquí se podría notificar al sistema de input
                // InputManager::HandleMouseMove(deltaX, deltaY);
            }

            lastX = currentX;
            lastY = currentY;
            firstMouse = false;
        }
        return 0;

    case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            (void)delta; // Suppress unused variable warning
            // InputManager::HandleMouseWheel(delta);
        }
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
            (void)hdc; // Suppress unused variable warning
            // No dibujar nada aquí - DirectX se encarga del rendering
            EndPaint(m_hwnd, &ps);
        }
        return 0;

    case WM_ERASEBKGND:
        // No borrar fondo - DirectX se encarga
        return 1;

    default:
        return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}
