#pragma once
#include "Resource.h"

enum VertexAttribute : uint32_t
{
	POSITION = 1 << 0,
	UV       = 1 << 1,
	NORMAL   = 1 << 2,
	TANGENT  = 1 << 3,
	SKIN     = 1 << 4,
	COLOR    = 1 << 5,
};

class Mesh final : public Resource<Mesh>
{
public:
	Mesh(class Context* context);
	~Mesh() = default;

	void Create(Geometry& geometry);

	void LoadFromFile(const std::string& path) override;

	const unsigned int GetIndexCount() const { return m_numIndices; }
	const unsigned int GetVertexCount() const { return m_numVertices; }

	ID3D11Buffer* GetIndices() const { return m_indices.Get(); }
	ID3D11Buffer* GetPositions() const { return m_positions.Get(); }

	void BindIndex(const UINT& offset = 0);
	void BindPosition(const UINT& slot, const UINT& offset = 0, const UINT& stride = sizeof(Vector3));
	void BindNormal(const UINT& slot, const UINT& offset = 0, const UINT& stride = sizeof(Vector3));

public:
	uint32_t m_numVertices;
	uint32_t m_numIndices;
	uint32_t m_attribute;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_positions;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_normals;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_binormals;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_tangents;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_uvs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_colors;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_blendIndices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_blendWeights;

	class Graphics* m_graphics;
};