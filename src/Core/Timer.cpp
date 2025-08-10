#include "Timer.h"
#include <algorithm>

Timer::Timer()
    : m_deltaTime(0.0f)
    , m_totalTime(0.0f)
    , m_timeScale(1.0f)
    , m_fps(0.0f)
    , m_frameCount(0)
    , m_fpsTimer(0.0f)
    , m_fpsFrameCount(0)
    , m_isRunning(false)
{
    // Obtener frecuencia del contador de alta resolución
    QueryPerformanceFrequency(&m_frequency);

    // Inicializar contadores
    QueryPerformanceCounter(&m_startTime);
    m_currentTime = m_startTime;
    m_lastTime = m_startTime;
}

void Timer::Start()
{
    if (!m_isRunning)
    {
        QueryPerformanceCounter(&m_startTime);
        m_currentTime = m_startTime;
        m_lastTime = m_startTime;
        m_isRunning = true;
        m_totalTime = 0.0f;
        m_frameCount = 0;
        m_fpsTimer = 0.0f;
        m_fpsFrameCount = 0;
    }
}

void Timer::Update()
{
    if (!m_isRunning)
        return;

    // Guardar tiempo anterior
    m_lastTime = m_currentTime;

    // Obtener tiempo actual
    QueryPerformanceCounter(&m_currentTime);

    // Calcular delta time en segundos
    float rawDeltaTime = static_cast<float>(m_currentTime.QuadPart - m_lastTime.QuadPart) /
                        static_cast<float>(m_frequency.QuadPart);

    // Aplicar escala de tiempo
    m_deltaTime = rawDeltaTime * m_timeScale;

    // Limitar delta time para evitar saltos grandes
    m_deltaTime = std::min(m_deltaTime, 1.0f / 15.0f); // Máximo 15 FPS

    // Actualizar tiempo total
    m_totalTime += m_deltaTime;

    // Actualizar contador de frames
    m_frameCount++;

    // Actualizar FPS
    UpdateFPS();
}

void Timer::UpdateFPS()
{
    m_fpsTimer += m_deltaTime;
    m_fpsFrameCount++;

    // Calcular FPS cada segundo
    if (m_fpsTimer >= 1.0f)
    {
        m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsTimer;
        m_fpsTimer = 0.0f;
        m_fpsFrameCount = 0;
    }
}

void Timer::Stop()
{
    m_isRunning = false;
}

void Timer::Reset()
{
    QueryPerformanceCounter(&m_startTime);
    m_currentTime = m_startTime;
    m_lastTime = m_startTime;
    m_totalTime = 0.0f;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_fpsTimer = 0.0f;
    m_fpsFrameCount = 0;
    m_fps = 0.0f;
}
