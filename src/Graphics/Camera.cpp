#include "Camera.h"
#include <algorithm>

Camera::Camera()
    : m_position(0.0f, 0.0f, -5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_roll(0.0f)
    , m_orbitTarget(0.0f, 0.0f, 0.0f)
    , m_orbitDistance(5.0f)
    , m_fov(D3DX_PI / 4.0f)
    , m_aspectRatio(1.33f)
    , m_nearZ(0.1f)
    , m_farZ(100.0f)
    , m_orthographic(false)
    , m_cameraType(CameraType::FREE_LOOK)
    , m_viewMatrixDirty(true)
    , m_projectionMatrixDirty(true)
    , m_frustumDirty(true)
{
    D3DXMatrixIdentity(&m_viewMatrix);
    D3DXMatrixIdentity(&m_projectionMatrix);
}

void Camera::Initialize(float fov, float aspect, float nearZ, float farZ)
{
    m_fov = fov;
    m_aspectRatio = aspect;
    m_nearZ = nearZ;
    m_farZ = farZ;

    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

void Camera::SetPosition(const D3DXVECTOR3& position)
{
    m_position = position;
    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::SetTarget(const D3DXVECTOR3& target)
{
    m_target = target;
    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::SetUp(const D3DXVECTOR3& up)
{
    m_up = up;
    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::LookAt(const D3DXVECTOR3& position, const D3DXVECTOR3& target, const D3DXVECTOR3& up)
{
    m_position = position;
    m_target = target;
    m_up = up;
    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::Move(const D3DXVECTOR3& direction, float distance)
{
    D3DXVECTOR3 normalizedDir;
    D3DXVec3Normalize(&normalizedDir, &direction);
    m_position += normalizedDir * distance;

    if (m_cameraType == CameraType::FREE_LOOK)
    {
        m_target += normalizedDir * distance;
    }

    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::MoveForward(float distance)
{
    D3DXVECTOR3 forward = GetForward();
    Move(forward, distance);
}

void Camera::MoveBackward(float distance)
{
    MoveForward(-distance);
}

void Camera::MoveLeft(float distance)
{
    D3DXVECTOR3 right = GetRight();
    Move(right, -distance);
}

void Camera::MoveRight(float distance)
{
    D3DXVECTOR3 right = GetRight();
    Move(right, distance);
}

void Camera::MoveUp(float distance)
{
    Move(m_up, distance);
}

void Camera::MoveDown(float distance)
{
    Move(m_up, -distance);
}

void Camera::Rotate(float yaw, float pitch, float roll)
{
    m_yaw += yaw;
    m_pitch += pitch;
    m_roll += roll;

    ConstrainAngles();

    if (m_cameraType == CameraType::FREE_LOOK)
    {
        // Recalcular target basado en los ángulos
        D3DXVECTOR3 forward;
        forward.x = cosf(m_pitch) * sinf(m_yaw);
        forward.y = sinf(m_pitch);
        forward.z = cosf(m_pitch) * cosf(m_yaw);

        m_target = m_position + forward;
    }

    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::RotateX(float angle)
{
    Rotate(0.0f, angle, 0.0f);
}

void Camera::RotateY(float angle)
{
    Rotate(angle, 0.0f, 0.0f);
}

void Camera::RotateZ(float angle)
{
    Rotate(0.0f, 0.0f, angle);
}

void Camera::HandleMouseMove(int deltaX, int deltaY, float sensitivity)
{
    if (m_cameraType == CameraType::FREE_LOOK)
    {
        Rotate(deltaX * sensitivity, -deltaY * sensitivity, 0.0f);
    }
    else if (m_cameraType == CameraType::ORBIT)
    {
        OrbitHorizontal(deltaX * sensitivity);
        OrbitVertical(-deltaY * sensitivity);
    }
}

void Camera::HandleKeyboard(bool forward, bool backward, bool left, bool right,
                           bool up, bool down, float deltaTime, float speed)
{
    float distance = speed * deltaTime;

    if (forward) MoveForward(distance);
    if (backward) MoveBackward(distance);
    if (left) MoveLeft(distance);
    if (right) MoveRight(distance);
    if (up) MoveUp(distance);
    if (down) MoveDown(distance);
}

void Camera::SetOrbitTarget(const D3DXVECTOR3& target)
{
    m_orbitTarget = target;
    m_cameraType = CameraType::ORBIT;
    m_viewMatrixDirty = true;
}

void Camera::SetOrbitDistance(float distance)
{
    m_orbitDistance = distance;
    if (m_cameraType == CameraType::ORBIT)
    {
        m_viewMatrixDirty = true;
    }
}

void Camera::OrbitHorizontal(float angle)
{
    if (m_cameraType != CameraType::ORBIT)
        return;

    m_yaw += angle;
    ConstrainAngles();

    // Recalcular posición orbital
    float x = m_orbitTarget.x + m_orbitDistance * cosf(m_pitch) * sinf(m_yaw);
    float y = m_orbitTarget.y + m_orbitDistance * sinf(m_pitch);
    float z = m_orbitTarget.z + m_orbitDistance * cosf(m_pitch) * cosf(m_yaw);

    m_position = D3DXVECTOR3(x, y, z);
    m_target = m_orbitTarget;
    m_viewMatrixDirty = true;
}

void Camera::OrbitVertical(float angle)
{
    if (m_cameraType != CameraType::ORBIT)
        return;

    m_pitch += angle;
    ConstrainAngles();

    // Recalcular posición orbital
    float x = m_orbitTarget.x + m_orbitDistance * cosf(m_pitch) * sinf(m_yaw);
    float y = m_orbitTarget.y + m_orbitDistance * sinf(m_pitch);
    float z = m_orbitTarget.z + m_orbitDistance * cosf(m_pitch) * cosf(m_yaw);

    m_position = D3DXVECTOR3(x, y, z);
    m_target = m_orbitTarget;
    m_viewMatrixDirty = true;
}

void Camera::SetPerspective(float fov, float aspect, float nearZ, float farZ)
{
    m_fov = fov;
    m_aspectRatio = aspect;
    m_nearZ = nearZ;
    m_farZ = farZ;
    m_orthographic = false;
    m_projectionMatrixDirty = true;
    m_frustumDirty = true;
}

void Camera::SetOrthographic(float width, float height, float nearZ, float farZ)
{
    m_aspectRatio = width / height;
    m_nearZ = nearZ;
    m_farZ = farZ;
    m_orthographic = true;
    m_projectionMatrixDirty = true;
    m_frustumDirty = true;
}

D3DXVECTOR3 Camera::GetForward() const
{
    D3DXVECTOR3 forward;
    D3DXVec3Subtract(&forward, &m_target, &m_position);
    D3DXVec3Normalize(&forward, &forward);
    return forward;
}

D3DXVECTOR3 Camera::GetRight() const
{
    D3DXVECTOR3 forward = GetForward();
    D3DXVECTOR3 right;
    D3DXVec3Cross(&right, &forward, &m_up);
    D3DXVec3Normalize(&right, &right);
    return right;
}

D3DXMATRIX Camera::GetViewProjectionMatrix() const
{
    return m_viewMatrix * m_projectionMatrix;
}

void Camera::Update()
{
    if (m_viewMatrixDirty)
    {
        UpdateViewMatrix();
    }

    if (m_projectionMatrixDirty)
    {
        UpdateProjectionMatrix();
    }

    if (m_frustumDirty)
    {
        UpdateFrustum();
    }
}

void Camera::UpdateViewMatrix()
{
    D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_target, &m_up);
    m_viewMatrixDirty = false;
    m_frustumDirty = true;
}

void Camera::UpdateProjectionMatrix()
{
    if (m_orthographic)
    {
        float width = 2.0f * m_orbitDistance;
        float height = width / m_aspectRatio;
        D3DXMatrixOrthoLH(&m_projectionMatrix, width, height, m_nearZ, m_farZ);
    }
    else
    {
        D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, m_fov, m_aspectRatio, m_nearZ, m_farZ);
    }

    m_projectionMatrixDirty = false;
    m_frustumDirty = true;
}

void Camera::UpdateFrustum()
{
    // Calcular planos del frustum para culling
    D3DXMATRIX viewProj = GetViewProjectionMatrix();

    // Plano izquierdo
    m_frustumPlanes[0].a = viewProj._14 + viewProj._11;
    m_frustumPlanes[0].b = viewProj._24 + viewProj._21;
    m_frustumPlanes[0].c = viewProj._34 + viewProj._31;
    m_frustumPlanes[0].d = viewProj._44 + viewProj._41;

    // Plano derecho
    m_frustumPlanes[1].a = viewProj._14 - viewProj._11;
    m_frustumPlanes[1].b = viewProj._24 - viewProj._21;
    m_frustumPlanes[1].c = viewProj._34 - viewProj._31;
    m_frustumPlanes[1].d = viewProj._44 - viewProj._41;

    // Plano superior
    m_frustumPlanes[2].a = viewProj._14 - viewProj._12;
    m_frustumPlanes[2].b = viewProj._24 - viewProj._22;
    m_frustumPlanes[2].c = viewProj._34 - viewProj._32;
    m_frustumPlanes[2].d = viewProj._44 - viewProj._42;

    // Plano inferior
    m_frustumPlanes[3].a = viewProj._14 + viewProj._12;
    m_frustumPlanes[3].b = viewProj._24 + viewProj._22;
    m_frustumPlanes[3].c = viewProj._34 + viewProj._32;
    m_frustumPlanes[3].d = viewProj._44 + viewProj._42;

    // Plano cercano
    m_frustumPlanes[4].a = viewProj._13;
    m_frustumPlanes[4].b = viewProj._23;
    m_frustumPlanes[4].c = viewProj._33;
    m_frustumPlanes[4].d = viewProj._43;

    // Plano lejano
    m_frustumPlanes[5].a = viewProj._14 - viewProj._13;
    m_frustumPlanes[5].b = viewProj._24 - viewProj._23;
    m_frustumPlanes[5].c = viewProj._34 - viewProj._33;
    m_frustumPlanes[5].d = viewProj._44 - viewProj._43;

    // Normalizar planos
    for (int i = 0; i < 6; i++)
    {
        D3DXPlaneNormalize(&m_frustumPlanes[i], &m_frustumPlanes[i]);
    }

    m_frustumDirty = false;
}

void Camera::ConstrainAngles()
{
    // Limitar pitch para evitar gimbal lock
    const float maxPitch = D3DX_PI / 2.0f - 0.01f;
    m_pitch = std::max(-maxPitch, std::min(maxPitch, m_pitch));

    // Normalizar yaw
    while (m_yaw > D3DX_PI) m_yaw -= 2.0f * D3DX_PI;
    while (m_yaw < -D3DX_PI) m_yaw += 2.0f * D3DX_PI;
}

bool Camera::IsPointInFrustum(const D3DXVECTOR3& point) const
{
    for (int i = 0; i < 6; i++)
    {
        if (D3DXPlaneDotCoord(&m_frustumPlanes[i], &point) < 0.0f)
        {
            return false;
        }
    }
    return true;
}

bool Camera::IsSphereInFrustum(const D3DXVECTOR3& center, float radius) const
{
    for (int i = 0; i < 6; i++)
    {
        if (D3DXPlaneDotCoord(&m_frustumPlanes[i], &center) < -radius)
        {
            return false;
        }
    }
    return true;
}

void Camera::Reset()
{
    m_position = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
    m_target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    m_roll = 0.0f;
    m_viewMatrixDirty = true;
    m_frustumDirty = true;
}
