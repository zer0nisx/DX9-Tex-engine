#pragma once

#include <windows.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    bool Create(int width, int height, const std::string& title);
    void Destroy();
    void ProcessMessages();

    // Getters
    HWND GetHandle() const { return m_hwnd; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool ShouldClose() const { return m_shouldClose; }
    bool IsActive() const { return m_isActive; }

    // Window state
    void SetActive(bool active) { m_isActive = active; }
    void SetShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    HINSTANCE m_hinstance;
    std::wstring m_className;
    std::string m_title;
    int m_width;
    int m_height;
    bool m_shouldClose;
    bool m_isActive;
};
