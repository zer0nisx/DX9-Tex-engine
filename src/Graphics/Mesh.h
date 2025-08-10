#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <memory>
#include <string>

class Material;

struct Vertex {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 texCoord0;
    D3DXVECTOR2 texCoord1;
    D3DXVECTOR3 tangent;
    D3DXVECTOR3 binormal;
    D3DCOLOR color;

    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_DIFFUSE;
};

struct SubMesh {
    DWORD startIndex;
    DWORD primitiveCount;
    std::shared_ptr<Material> material;
    D3DPRIMITIVETYPE primitiveType;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    // Creation
    bool CreateCube(IDirect3DDevice9* device, float size = 1.0f);
    bool CreateSphere(IDirect3DDevice9* device, float radius = 1.0f, int slices = 16, int stacks = 16);
    bool CreatePlane(IDirect3DDevice9* device, float width = 1.0f, float height = 1.0f, int subdivisions = 1);
    bool CreateCylinder(IDirect3DDevice9* device, float radius = 0.5f, float height = 1.0f, int slices = 16);
    bool CreateFromFile(IDirect3DDevice9* device, const std::string& filename);

    // Manual creation
    bool CreateFromData(IDirect3DDevice9* device, const std::vector<Vertex>& vertices,
                       const std::vector<DWORD>& indices);
    void Clear();

    // Vertex/Index manipulation
    void AddVertex(const Vertex& vertex);
    void AddTriangle(DWORD index1, DWORD index2, DWORD index3);
    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<DWORD>& indices);

    // Buffer management
    bool CreateBuffers(IDirect3DDevice9* device);
    void ReleaseBuffers();
    bool UpdateVertexBuffer();
    bool UpdateIndexBuffer();

    // Rendering
    void Render(IDirect3DDevice9* device) const;
    void RenderSubMesh(IDirect3DDevice9* device, int subMeshIndex) const;
    void SetupStreamSource(IDirect3DDevice9* device) const;

    // Materials
    void SetMaterial(std::shared_ptr<Material> material, int subMeshIndex = 0);
    std::shared_ptr<Material> GetMaterial(int subMeshIndex = 0) const;
    void AddSubMesh(const SubMesh& subMesh);
    int GetSubMeshCount() const { return static_cast<int>(m_subMeshes.size()); }

    // Properties
    int GetVertexCount() const { return static_cast<int>(m_vertices.size()); }
    int GetIndexCount() const { return static_cast<int>(m_indices.size()); }
    int GetTriangleCount() const { return GetIndexCount() / 3; }
    const std::vector<Vertex>& GetVertices() const { return m_vertices; }
    const std::vector<DWORD>& GetIndices() const { return m_indices; }

    // Bounding volume
    void CalculateBounds();
    const D3DXVECTOR3& GetBoundsMin() const { return m_boundsMin; }
    const D3DXVECTOR3& GetBoundsMax() const { return m_boundsMax; }
    D3DXVECTOR3 GetBoundsCenter() const;
    float GetBoundsRadius() const;

    // Transformation
    void Transform(const D3DXMATRIX& matrix);
    void Translate(const D3DXVECTOR3& translation);
    void Scale(float scale);
    void Scale(const D3DXVECTOR3& scale);

    // Normal calculation
    void CalculateNormals();
    void CalculateTangents();
    void SmoothNormals();

    // UV manipulation
    void SetUVChannel(int channel, const std::vector<D3DXVECTOR2>& uvs);
    void GenerateUVs(int channel = 0);
    void ScaleUVs(float scaleU, float scaleV, int channel = 0);
    void OffsetUVs(float offsetU, float offsetV, int channel = 0);

    // Utility
    bool IsValid() const;
    void FlipNormals();
    void FlipWindingOrder();
    Vertex GetVertex(int index) const;
    void SetVertex(int index, const Vertex& vertex);

    // Optimization
    void OptimizeVertices();
    void GenerateLOD(float reductionFactor);

private:
    void GenerateCubeData(float size);
    void GenerateSphereData(float radius, int slices, int stacks);
    void GeneratePlaneData(float width, float height, int subdivisions);
    void GenerateCylinderData(float radius, float height, int slices);

    IDirect3DDevice9* m_device;
    IDirect3DVertexBuffer9* m_vertexBuffer;
    IDirect3DIndexBuffer9* m_indexBuffer;

    std::vector<Vertex> m_vertices;
    std::vector<DWORD> m_indices;
    std::vector<SubMesh> m_subMeshes;

    D3DXVECTOR3 m_boundsMin;
    D3DXVECTOR3 m_boundsMax;

    bool m_buffersDirty;
    D3DPRIMITIVETYPE m_primitiveType;
};
