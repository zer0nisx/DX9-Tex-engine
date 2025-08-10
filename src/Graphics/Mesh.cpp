#include "Mesh.h"
#include "../Textures/Material.h"
#include <iostream>

Mesh::Mesh()
    : m_device(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
    , m_boundsMin(0.0f, 0.0f, 0.0f)
    , m_boundsMax(0.0f, 0.0f, 0.0f)
    , m_buffersDirty(true)
    , m_primitiveType(D3DPT_TRIANGLELIST)
{
}

Mesh::~Mesh()
{
    ReleaseBuffers();
}

bool Mesh::CreateCube(IDirect3DDevice9* device, float size)
{
    if (!device)
    {
        std::cerr << "Invalid device pointer!" << std::endl;
        return false;
    }

    m_device = device;
    GenerateCubeData(size);

    if (!CreateBuffers(device))
    {
        std::cerr << "Failed to create cube buffers!" << std::endl;
        return false;
    }

    CalculateBounds();

    // Crear submesh por defecto
    SubMesh subMesh;
    subMesh.startIndex = 0;
    subMesh.primitiveCount = GetTriangleCount();
    subMesh.primitiveType = D3DPT_TRIANGLELIST;
    subMesh.material = Material::CreateDefaultMaterial();
    m_subMeshes.push_back(subMesh);

    std::cout << "Created cube mesh: " << GetVertexCount() << " vertices, "
              << GetTriangleCount() << " triangles" << std::endl;
    return true;
}

void Mesh::GenerateCubeData(float size)
{
    Clear();

    float halfSize = size * 0.5f;

    // Definir vertices del cubo con normales y UVs
    Vertex vertices[24]; // 4 vertices por cara, 6 caras

    // Cara frontal (Z+)
    vertices[0] = {{-halfSize, -halfSize, halfSize}, {0, 0, 1}, {0, 1}, {0, 0}, {1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[1] = {{halfSize, -halfSize, halfSize}, {0, 0, 1}, {1, 1}, {0, 0}, {1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[2] = {{halfSize, halfSize, halfSize}, {0, 0, 1}, {1, 0}, {0, 0}, {1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[3] = {{-halfSize, halfSize, halfSize}, {0, 0, 1}, {0, 0}, {0, 0}, {1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Cara trasera (Z-)
    vertices[4] = {{halfSize, -halfSize, -halfSize}, {0, 0, -1}, {0, 1}, {0, 0}, {-1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[5] = {{-halfSize, -halfSize, -halfSize}, {0, 0, -1}, {1, 1}, {0, 0}, {-1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[6] = {{-halfSize, halfSize, -halfSize}, {0, 0, -1}, {1, 0}, {0, 0}, {-1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[7] = {{halfSize, halfSize, -halfSize}, {0, 0, -1}, {0, 0}, {0, 0}, {-1, 0, 0}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Cara izquierda (X-)
    vertices[8] = {{-halfSize, -halfSize, -halfSize}, {-1, 0, 0}, {0, 1}, {0, 0}, {0, 0, 1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[9] = {{-halfSize, -halfSize, halfSize}, {-1, 0, 0}, {1, 1}, {0, 0}, {0, 0, 1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[10] = {{-halfSize, halfSize, halfSize}, {-1, 0, 0}, {1, 0}, {0, 0}, {0, 0, 1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[11] = {{-halfSize, halfSize, -halfSize}, {-1, 0, 0}, {0, 0}, {0, 0}, {0, 0, 1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Cara derecha (X+)
    vertices[12] = {{halfSize, -halfSize, halfSize}, {1, 0, 0}, {0, 1}, {0, 0}, {0, 0, -1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[13] = {{halfSize, -halfSize, -halfSize}, {1, 0, 0}, {1, 1}, {0, 0}, {0, 0, -1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[14] = {{halfSize, halfSize, -halfSize}, {1, 0, 0}, {1, 0}, {0, 0}, {0, 0, -1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[15] = {{halfSize, halfSize, halfSize}, {1, 0, 0}, {0, 0}, {0, 0}, {0, 0, -1}, {0, 1, 0}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Cara inferior (Y-)
    vertices[16] = {{-halfSize, -halfSize, -halfSize}, {0, -1, 0}, {0, 1}, {0, 0}, {1, 0, 0}, {0, 0, 1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[17] = {{halfSize, -halfSize, -halfSize}, {0, -1, 0}, {1, 1}, {0, 0}, {1, 0, 0}, {0, 0, 1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[18] = {{halfSize, -halfSize, halfSize}, {0, -1, 0}, {1, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[19] = {{-halfSize, -halfSize, halfSize}, {0, -1, 0}, {0, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Cara superior (Y+)
    vertices[20] = {{-halfSize, halfSize, halfSize}, {0, 1, 0}, {0, 1}, {0, 0}, {1, 0, 0}, {0, 0, -1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[21] = {{halfSize, halfSize, halfSize}, {0, 1, 0}, {1, 1}, {0, 0}, {1, 0, 0}, {0, 0, -1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[22] = {{halfSize, halfSize, -halfSize}, {0, 1, 0}, {1, 0}, {0, 0}, {1, 0, 0}, {0, 0, -1}, D3DCOLOR_ARGB(255, 255, 255, 255)};
    vertices[23] = {{-halfSize, halfSize, -halfSize}, {0, 1, 0}, {0, 0}, {0, 0}, {1, 0, 0}, {0, 0, -1}, D3DCOLOR_ARGB(255, 255, 255, 255)};

    // Añadir vertices
    for (int i = 0; i < 24; i++)
    {
        AddVertex(vertices[i]);
    }

    // Definir índices (2 triángulos por cara)
    DWORD indices[] = {
        // Frontal
        0, 1, 2,  2, 3, 0,
        // Trasera
        4, 5, 6,  6, 7, 4,
        // Izquierda
        8, 9, 10,  10, 11, 8,
        // Derecha
        12, 13, 14,  14, 15, 12,
        // Inferior
        16, 17, 18,  18, 19, 16,
        // Superior
        20, 21, 22,  22, 23, 20
    };

    for (int i = 0; i < 36; i++)
    {
        m_indices.push_back(indices[i]);
    }
}

bool Mesh::CreateBuffers(IDirect3DDevice9* device)
{
    if (!device || m_vertices.empty() || m_indices.empty())
        return false;

    ReleaseBuffers();

    // Crear vertex buffer
    HRESULT hr = device->CreateVertexBuffer(
        static_cast<UINT>(m_vertices.size() * sizeof(Vertex)),
        D3DUSAGE_WRITEONLY,
        Vertex::FVF,
        D3DPOOL_MANAGED,
        &m_vertexBuffer,
        nullptr
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create vertex buffer! HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    // Llenar vertex buffer
    void* vertexData;
    hr = m_vertexBuffer->Lock(0, 0, &vertexData, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(vertexData, m_vertices.data(), m_vertices.size() * sizeof(Vertex));
        m_vertexBuffer->Unlock();
    }
    else
    {
        std::cerr << "Failed to lock vertex buffer!" << std::endl;
        return false;
    }

    // Crear index buffer
    hr = device->CreateIndexBuffer(
        static_cast<UINT>(m_indices.size() * sizeof(DWORD)),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &m_indexBuffer,
        nullptr
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create index buffer! HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    // Llenar index buffer
    void* indexData;
    hr = m_indexBuffer->Lock(0, 0, &indexData, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(indexData, m_indices.data(), m_indices.size() * sizeof(DWORD));
        m_indexBuffer->Unlock();
    }
    else
    {
        std::cerr << "Failed to lock index buffer!" << std::endl;
        return false;
    }

    m_buffersDirty = false;
    return true;
}

void Mesh::Render(IDirect3DDevice9* device) const
{
    if (!device || !m_vertexBuffer || !m_indexBuffer)
        return;

    // Configurar stream source
    SetupStreamSource(device);

    // Renderizar todos los submeshes
    for (const auto& subMesh : m_subMeshes)
    {
        device->DrawIndexedPrimitive(
            subMesh.primitiveType,
            0,                              // base vertex index
            0,                              // min vertex index
            GetVertexCount(),               // num vertices
            subMesh.startIndex,             // start index
            subMesh.primitiveCount          // primitive count
        );
    }
}

void Mesh::SetupStreamSource(IDirect3DDevice9* device) const
{
    if (!device || !m_vertexBuffer || !m_indexBuffer)
        return;

    device->SetStreamSource(0, m_vertexBuffer, 0, sizeof(Vertex));
    device->SetIndices(m_indexBuffer);
    device->SetFVF(Vertex::FVF);
}

void Mesh::SetMaterial(std::shared_ptr<Material> material, int subMeshIndex)
{
    if (subMeshIndex >= 0 && subMeshIndex < static_cast<int>(m_subMeshes.size()))
    {
        m_subMeshes[subMeshIndex].material = material;
    }
    else if (subMeshIndex == 0 && m_subMeshes.empty())
    {
        // Crear submesh si no existe
        SubMesh subMesh;
        subMesh.startIndex = 0;
        subMesh.primitiveCount = GetTriangleCount();
        subMesh.primitiveType = D3DPT_TRIANGLELIST;
        subMesh.material = material;
        m_subMeshes.push_back(subMesh);
    }
}

std::shared_ptr<Material> Mesh::GetMaterial(int subMeshIndex) const
{
    if (subMeshIndex >= 0 && subMeshIndex < static_cast<int>(m_subMeshes.size()))
    {
        return m_subMeshes[subMeshIndex].material;
    }
    return nullptr;
}

void Mesh::AddVertex(const Vertex& vertex)
{
    m_vertices.push_back(vertex);
    m_buffersDirty = true;
}

void Mesh::AddTriangle(DWORD index1, DWORD index2, DWORD index3)
{
    m_indices.push_back(index1);
    m_indices.push_back(index2);
    m_indices.push_back(index3);
    m_buffersDirty = true;
}

void Mesh::CalculateBounds()
{
    if (m_vertices.empty())
    {
        m_boundsMin = m_boundsMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        return;
    }

    m_boundsMin = m_boundsMax = m_vertices[0].position;

    for (const auto& vertex : m_vertices)
    {
        // Min bounds
        m_boundsMin.x = std::min(m_boundsMin.x, vertex.position.x);
        m_boundsMin.y = std::min(m_boundsMin.y, vertex.position.y);
        m_boundsMin.z = std::min(m_boundsMin.z, vertex.position.z);

        // Max bounds
        m_boundsMax.x = std::max(m_boundsMax.x, vertex.position.x);
        m_boundsMax.y = std::max(m_boundsMax.y, vertex.position.y);
        m_boundsMax.z = std::max(m_boundsMax.z, vertex.position.z);
    }
}

D3DXVECTOR3 Mesh::GetBoundsCenter() const
{
    return (m_boundsMin + m_boundsMax) * 0.5f;
}

float Mesh::GetBoundsRadius() const
{
    D3DXVECTOR3 size = m_boundsMax - m_boundsMin;
    return D3DXVec3Length(&size) * 0.5f;
}

void Mesh::Clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_subMeshes.clear();
    ReleaseBuffers();
    m_buffersDirty = true;
}

void Mesh::ReleaseBuffers()
{
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }
}

bool Mesh::IsValid() const
{
    return !m_vertices.empty() && !m_indices.empty() &&
           m_vertexBuffer != nullptr && m_indexBuffer != nullptr;
}
