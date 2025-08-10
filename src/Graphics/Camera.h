#pragma once

#include <d3d9.h>
#include <d3dx9.h>

enum class CameraType {
    FREE_LOOK,
    ORBIT,
    FIRST_PERSON,
    THIRD_PERSON
};

class Camera {
public:
    Camera();
    ~Camera() = default;

    // Initialization
    void Initialize(float fov = D3DX_PI / 4.0f, float aspect = 1.33f, float nearZ = 0.1f, float farZ = 1000.0f);

    // Position and orientation
    void SetPosition(const D3DXVECTOR3& position);
    void SetTarget(const D3DXVECTOR3& target);
    void SetUp(const D3DXVECTOR3& up);
    void LookAt(const D3DXVECTOR3& position, const D3DXVECTOR3& target, const D3DXVECTOR3& up);

    // Movement
    void Move(const D3DXVECTOR3& direction, float distance);
    void MoveForward(float distance);
    void MoveBackward(float distance);
    void MoveLeft(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void MoveDown(float distance);

    // Rotation
    void Rotate(float yaw, float pitch, float roll = 0.0f);
    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    void RotateAroundTarget(float yaw, float pitch);

    // Orbit camera
    void SetOrbitTarget(const D3DXVECTOR3& target);
    void SetOrbitDistance(float distance);
    void OrbitHorizontal(float angle);
    void OrbitVertical(float angle);
    void ZoomIn(float amount);
    void ZoomOut(float amount);

    // Projection
    void SetPerspective(float fov, float aspect, float nearZ, float farZ);
    void SetOrthographic(float width, float height, float nearZ, float farZ);
    void SetFOV(float fov);
    void SetAspectRatio(float aspect);
    void SetNearFar(float nearZ, float farZ);

    // Matrix access
    const D3DXMATRIX& GetViewMatrix() const { return m_viewMatrix; }
    const D3DXMATRIX& GetProjectionMatrix() const { return m_projectionMatrix; }
    D3DXMATRIX GetViewProjectionMatrix() const;

    // Properties
    const D3DXVECTOR3& GetPosition() const { return m_position; }
    const D3DXVECTOR3& GetTarget() const { return m_target; }
    const D3DXVECTOR3& GetUp() const { return m_up; }
    D3DXVECTOR3 GetForward() const;
    D3DXVECTOR3 GetRight() const;

    float GetFOV() const { return m_fov; }
    float GetAspectRatio() const { return m_aspectRatio; }
    float GetNearPlane() const { return m_nearZ; }
    float GetFarPlane() const { return m_farZ; }

    // Camera type
    void SetCameraType(CameraType type) { m_cameraType = type; }
    CameraType GetCameraType() const { return m_cameraType; }

    // Input handling
    void HandleMouseMove(int deltaX, int deltaY, float sensitivity = 0.005f);
    void HandleMouseWheel(int delta, float sensitivity = 1.0f);
    void HandleKeyboard(bool forward, bool backward, bool left, bool right,
                       bool up, bool down, float deltaTime, float speed = 10.0f);

    // Utility
    void Update();
    void Reset();
    D3DXVECTOR3 ScreenToWorld(int screenX, int screenY, int screenWidth, int screenHeight, float depth = 1.0f) const;
    D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3& worldPos, int screenWidth, int screenHeight) const;

    // Frustum culling
    bool IsPointInFrustum(const D3DXVECTOR3& point) const;
    bool IsSphereInFrustum(const D3DXVECTOR3& center, float radius) const;
    bool IsBoxInFrustum(const D3DXVECTOR3& min, const D3DXVECTOR3& max) const;

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateFrustum();
    void ConstrainAngles();

    // Position and orientation
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_target;
    D3DXVECTOR3 m_up;

    // Rotation angles
    float m_yaw;
    float m_pitch;
    float m_roll;

    // Orbit camera
    D3DXVECTOR3 m_orbitTarget;
    float m_orbitDistance;

    // Projection parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearZ;
    float m_farZ;
    bool m_orthographic;

    // Matrices
    D3DXMATRIX m_viewMatrix;
    D3DXMATRIX m_projectionMatrix;

    // Camera type
    CameraType m_cameraType;

    // Frustum planes (for culling)
    D3DXPLANE m_frustumPlanes[6];

    // Flags
    bool m_viewMatrixDirty;
    bool m_projectionMatrixDirty;
    bool m_frustumDirty;
};
