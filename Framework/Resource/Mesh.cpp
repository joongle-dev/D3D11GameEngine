#include "Framework.h"
#include "Mesh.h"


using namespace DirectX;

Mesh::Mesh(Context * context) :
	Resource<Mesh>(context)
{
	mGraphics = context->GetSubsystem<Graphics>();
}

void Mesh::Create(Geometry & geometry)
{
	mName = "";
	mNumVertices = geometry.positions.size();
	mNumIndices  = geometry.indices.size();
	mAttributes   = 0;
	mAttributes  |= geometry.normals.size() == mNumVertices ? VertexAttribute::NORMAL : 0;
	mAttributes  |= geometry.tangents.size() == mNumVertices ? VertexAttribute::TANGENT : 0;
	mAttributes  |= geometry.uvs.size()     == mNumVertices ? VertexAttribute::TEXCOORD : 0;

	//Create index buffer.
	{
		mGraphics->CreateBuffer(
			mIndices.ReleaseAndGetAddressOf(),
			D3D11_BIND_INDEX_BUFFER,
			mNumIndices * sizeof(uint32_t),
			geometry.indices.data());
	}

	//Create position attribute buffer
	{
		mGraphics->CreateBuffer(
			mPositions.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			geometry.positions.data());
	}

	//Create normal attribute buffer
	if (mAttributes & VertexAttribute::NORMAL)
	{
		mGraphics->CreateBuffer(
			mNormals.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			geometry.normals .data());
	}

	//Create tangent attribute buffer
	if (mAttributes & VertexAttribute::TANGENT)
	{
		mGraphics->CreateBuffer(
			mTangents.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			geometry.tangents.data());
		mGraphics->CreateBuffer(
			mBinormals .ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			geometry.binormals.data());
	}

	//Create UV attribute buffer
	if (mAttributes & VertexAttribute::TEXCOORD)
	{
		mGraphics->CreateBuffer(
			mTexcoords.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector2),
			geometry.uvs.data());
	}
}

void Mesh::LoadFromFile(const std::string & path)
{
	//Open file
	FileStreamRead file;
	file.Open(path);

	//Read mesh properties
	file.Read(mName);
	file.Read(mNumVertices);
	file.Read(mNumIndices);
	file.Read(mAttributes);

	//Create byte buffer to load data into
	size_t bufferSize = std::max(mNumIndices * sizeof(uint32_t), mNumVertices * sizeof(XMFLOAT4));
	void* buffer = new byte[bufferSize];

	//Create index buffer.
	{
		file.Read(buffer, mNumIndices * sizeof(uint32_t));
		mGraphics->CreateBuffer(
			mIndices.ReleaseAndGetAddressOf(),
			D3D11_BIND_INDEX_BUFFER,
			mNumIndices * sizeof(uint32_t),
			buffer);
	}

	//Create position attribute buffer
	{
		file.Read(buffer, mNumVertices * sizeof(Vector3));
		mGraphics->CreateBuffer(
			mPositions.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			buffer);
	}

	//Create normal attribute buffer
	if (mAttributes & VertexAttribute::NORMAL) 
	{
		file.Read(buffer, mNumVertices * sizeof(Vector3));
		mGraphics->CreateBuffer(
			mNormals.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector3), 
			buffer);
	}

	//Create tangent attribute buffer
	if (mAttributes & VertexAttribute::TANGENT) 
	{
		file.Read(buffer, mNumVertices * sizeof(Vector3));
		mGraphics->CreateBuffer(
			mTangents.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector3), 
			buffer);

		file.Read(buffer, mNumVertices * sizeof(Vector3));
		mGraphics->CreateBuffer(
			mBinormals.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector3), 
			buffer);
	}

	//Create UV attribute buffer
	if (mAttributes & VertexAttribute::TEXCOORD) 
	{
		file.Read(buffer, mNumVertices * sizeof(Vector2));
		mGraphics->CreateBuffer(
			mTexcoords.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector2), 
			buffer);
	}

	//Create color attribute buffer
	if (mAttributes & VertexAttribute::COLOR) 
	{
		file.Read(buffer, mNumVertices * sizeof(Vector4));
		mGraphics->CreateBuffer(
			mColors.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector4), 
			buffer);
	}

	//Create skin blending attribute buffer
	if (mAttributes & VertexAttribute::SKIN) 
	{
		file.Read(buffer, mNumVertices * sizeof(Vector4));
		mGraphics->CreateBuffer(
			mBlendIndices.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector4), 
			buffer);

		file.Read(buffer, mNumVertices * sizeof(Vector4));
		mGraphics->CreateBuffer(
			mBlendWeights.ReleaseAndGetAddressOf(), 
			D3D11_BIND_VERTEX_BUFFER, 
			mNumVertices * sizeof(Vector4), 
			buffer);
	}

	file.Close();
}