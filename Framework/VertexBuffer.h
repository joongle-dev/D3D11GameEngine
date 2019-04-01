#pragma once

class VertexBuffer final
{
public:
	VertexBuffer(class Context* context);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer& rhs) = delete;
	VertexBuffer& operator=(const VertexBuffer& rhs) = delete;

	ID3D11Buffer* GetBuffer() const { return buffer; }
	const size_t& GetVertexCount() const { return vertexCount; }

	template <typename T>
	void Create
	(
		const std::vector<T>& vertices,
		const D3D11_USAGE& usage = D3D11_USAGE_DEFAULT
	);

	void Clear();
	void BindPipeline();

private:
	class Graphics* graphics;

	ID3D11Buffer* buffer;
	size_t stride;
	size_t offset;
	size_t vertexCount;
};

template<typename T>
inline void VertexBuffer::Create(const std::vector<T>& vertices, const D3D11_USAGE & usage)
{
	assert(!vertices.empty());

	Clear();

	stride = sizeof(T);
	vertexCount = vertices.size();

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT: 
		desc.CPUAccessFlags = 0;
		break;
	case D3D11_USAGE_DYNAMIC: 
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		break;
	}

	desc.Usage = usage;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(T) * vertices.size();

	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(D3D11_SUBRESOURCE_DATA));
	subData.pSysMem = vertices.data();

	auto hr = graphics->GetDevice()->CreateBuffer
	(
		&desc,
		&subData,
		&buffer
	);
	assert(SUCCEEDED(hr));
}
