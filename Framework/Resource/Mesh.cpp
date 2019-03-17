#include "Framework.h"
#include "Mesh.h"


using namespace DirectX;

Mesh::Mesh(Context * context) :
	Resource<Mesh>(context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void Mesh::Create(Geometry & geometry)
{
	m_name = "";
	m_numVertices = geometry.positions.size();
	m_numIndices  = geometry.indices.size();
	m_attribute   = 0;
	m_attribute  |= geometry.normals.size() == m_numVertices ? VertexAttribute::NORMAL : 0;
	m_attribute  |= geometry.uvs.size()     == m_numVertices ? VertexAttribute::UV : 0;

	//Create index buffer.
	{
		m_graphics->CreateBuffer(
			m_indices.ReleaseAndGetAddressOf(),
			D3D11_BIND_INDEX_BUFFER,
			m_numIndices * sizeof(uint32_t),
			geometry.indices.data());
	}

	//Create position attribute buffer
	{
		m_graphics->CreateBuffer(
			m_positions.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			m_numVertices * sizeof(Vector3),
			geometry.positions.data());
	}

	//Create normal attribute buffer
	if (m_attribute & VertexAttribute::NORMAL)
	{
		m_graphics->CreateBuffer(
			m_normals.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			m_numVertices * sizeof(Vector3),
			geometry.normals .data());
	}

	//Create UV attribute buffer
	if (m_attribute & VertexAttribute::UV)
	{
		m_graphics->CreateBuffer(
			m_uvs.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			m_numVertices * sizeof(Vector2),
			geometry.uvs.data());
	}
}

void Mesh::LoadFromFile(std::string & path)
{
	//Open file
	FileStreamRead file;
	file.Open(path);

	//Read mesh properties
	file.Read(m_name);
	file.Read(m_numVertices);
	file.Read(m_numIndices);
	file.Read(m_attribute);

	//Create byte buffer to load data into
	size_t bufferSize = std::max(m_numIndices * sizeof(uint32_t), m_numVertices * sizeof(XMFLOAT4));
	void* buffer = new byte[bufferSize];

	//Create index buffer.
	{
		file.Read(buffer, m_numIndices * sizeof(uint32_t));
		m_graphics->CreateBuffer(
			m_indices.ReleaseAndGetAddressOf(),
			D3D11_BIND_INDEX_BUFFER,
			m_numIndices * sizeof(uint32_t),
			buffer);
	}

	//Create position attribute buffer
	{
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		m_graphics->CreateBuffer(
			m_positions.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			m_numVertices * sizeof(Vector3),
			buffer);
	}

	//Create normal attribute buffer
	if (m_attribute & VertexAttribute::NORMAL) 
	{
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		m_graphics->CreateBuffer(
			m_normals.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector3), 
			buffer);
	}

	//Create tangent attribute buffer
	if (m_attribute & VertexAttribute::TANGENT) 
	{
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		m_graphics->CreateBuffer(
			m_binormals.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector3), 
			buffer);

		file.Read(buffer, m_numVertices * sizeof(Vector3));
		m_graphics->CreateBuffer(
			m_tangents.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector3), 
			buffer);
	}

	//Create UV attribute buffer
	if (m_attribute & VertexAttribute::UV) 
	{
		file.Read(buffer, m_numVertices * sizeof(Vector2));
		m_graphics->CreateBuffer(
			m_uvs.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector2), 
			buffer);
	}

	//Create color attribute buffer
	if (m_attribute & VertexAttribute::COLOR) 
	{
		file.Read(buffer, m_numVertices * sizeof(Vector4));
		m_graphics->CreateBuffer(
			m_colors.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector4), 
			buffer);
	}

	//Create skin blending attribute buffer
	if (m_attribute & VertexAttribute::SKIN) 
	{
		file.Read(buffer, m_numVertices * sizeof(Vector4));
		m_graphics->CreateBuffer(
			m_blendIndices.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector4), 
			buffer);

		file.Read(buffer, m_numVertices * sizeof(Vector4));
		m_graphics->CreateBuffer(
			m_blendWeights.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			m_numVertices * sizeof(Vector4), 
			buffer);
	}

	file.Close();
}

void Mesh::BindIndex(const UINT & offset)
{
	m_graphics->GetDeviceContext()->IASetIndexBuffer(m_indices.Get(), DXGI_FORMAT_R32_UINT, offset);
}

void Mesh::BindPosition(const UINT & slot, const UINT & offset, const UINT & stride)
{
	m_graphics->GetDeviceContext()->IASetVertexBuffers(slot, 1, m_positions.GetAddressOf(), &stride, &offset);
}
