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

	//Initialize D3D buffer properties
	HRESULT hr;
	ID3D11Device* device = m_graphics->GetDevice();
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA bufferData;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT; //GPU access only
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;

	//Create index buffer.
	bufferData.pSysMem = geometry.indices.data();
	bufferDesc.ByteWidth = m_numIndices * sizeof(uint32_t);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, &bufferData, m_indices.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	//Create position attribute buffer
	bufferData.pSysMem = geometry.positions.data();
	bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, &bufferData, m_positions.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	//Create normal attribute buffer
	if (m_attribute & VertexAttribute::NORMAL) {
		bufferData.pSysMem = geometry.normals.data();
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_normals.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//Create UV attribute buffer
	if (m_attribute & VertexAttribute::UV) {
		bufferData.pSysMem = geometry.uvs.data();
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector2);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_uvs.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void Mesh::LoadFromFile(std::string& path)
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
	size_t bufferSize = max(m_numIndices * sizeof(uint32_t), m_numVertices * sizeof(XMFLOAT4));
	void* buffer = new byte[bufferSize];

	//Initialize D3D buffer properties
	HRESULT hr;
	ID3D11Device* device = m_graphics->GetDevice();
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA bufferData;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT; //GPU access only
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;
	bufferData.pSysMem = buffer;

	//Create index buffer.
	file.Read(buffer, m_numIndices * sizeof(uint32_t));
	bufferDesc.ByteWidth = m_numIndices * sizeof(uint32_t);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, &bufferData, m_indices.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	//Create position attribute buffer
	file.Read(buffer, m_numVertices * sizeof(Vector3));
	bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, &bufferData, m_positions.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	//Create normal attribute buffer
	if (m_attribute & VertexAttribute::NORMAL) {
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_normals.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//Create tangent attribute buffer
	if (m_attribute & VertexAttribute::TANGENT) {
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_binormals.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
		file.Read(buffer, m_numVertices * sizeof(Vector3));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector3);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_tangents.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//Create UV attribute buffer
	if (m_attribute & VertexAttribute::UV) {
		file.Read(buffer, m_numVertices * sizeof(Vector2));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector2);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_uvs.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//Create color attribute buffer
	if (m_attribute & VertexAttribute::COLOR) {
		file.Read(buffer, m_numVertices * sizeof(Vector4));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector4);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_colors.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//Create skin blending attribute buffer
	if (m_attribute & VertexAttribute::SKIN) {
		file.Read(buffer, m_numVertices * sizeof(Vector4));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector4);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_blendIndices.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
		file.Read(buffer, m_numVertices * sizeof(Vector4));
		bufferDesc.ByteWidth = m_numVertices * sizeof(Vector4);
		hr = device->CreateBuffer(&bufferDesc, &bufferData, m_blendWeights.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	file.Close();
}

void Mesh::BindIndex(UINT offset)
{
	m_graphics->GetDeviceContext()->IASetIndexBuffer(m_indices.Get(), DXGI_FORMAT_R32_UINT, offset);
}

void Mesh::BindPosition(UINT slot, UINT offset, UINT stride)
{
	m_graphics->GetDeviceContext()->IASetVertexBuffers(slot, 1, m_positions.GetAddressOf(), &stride, &offset);
}
