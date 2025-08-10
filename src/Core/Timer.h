#pragma once

#include <windows.h>

class Timer {
public:
    Timer();
    ~Timer() = default;

    void Start();
    void Update();
    void Stop();
    void Reset();

    // Time getters
    float GetDeltaTime() const { return m_deltaTime; }
    float GetTotalTime() const { return m_totalTime; }
    float GetFPS() const { return m_fps; }

    // Frame counting
    int GetFrameCount() const { return m_frameCount; }

    // Time scaling
    void SetTimeScale(float scale) { m_timeScale = scale; }
    float GetTimeScale() const { return m_timeScale; }

private:
    void UpdateFPS();

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_currentTime;
    LARGE_INTEGER m_lastTime;

    float m_deltaTime;
    float m_totalTime;
    float m_timeScale;

    // FPS calculation
    float m_fps;
    int m_frameCount;
    float m_fpsTimer;
    int m_fpsFrameCount;

    bool m_isRunning;
};
