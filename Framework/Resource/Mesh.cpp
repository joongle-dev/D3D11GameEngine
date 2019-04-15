#include "Framework.h"
#include "Mesh.h"


using namespace DirectX;

Mesh::Mesh(Context * context) :
	Resource<Mesh>(context),
	mNumVertices(0),
	mNumIndices(0),
	mAttributes(0)
{
	mGraphics = context->GetSubsystem<Graphics>();
}

void Mesh::Create(Geometry & geometry)
{
	mName = "";
	mNumVertices = geometry.positions.size();
	mNumIndices  = geometry.indices.size();
	mAttributes  = 0;
	mAttributes |= geometry.normals.size() == mNumVertices ? VertexAttribute::Normal : 0;
	mAttributes |= geometry.tangents.size() == mNumVertices ? VertexAttribute::Tangent : 0;
	mAttributes |= geometry.uvs.size() == mNumVertices ? VertexAttribute::Texcoord : 0;

	//Create index buffer.
	{
		mIndices.resize(mNumIndices);
		memcpy(mIndices.data(), geometry.indices.data(), mNumIndices * sizeof(uint32_t));
	}

	//Create position attribute buffer
	{
		mPositions.resize(mNumVertices);
		memcpy(mPositions.data(), geometry.positions.data(), mNumVertices * sizeof(Vector3));
	}

	//Create normal attribute buffer
	if (mAttributes & VertexAttribute::Normal)
	{
		mNormals.resize(mNumVertices);
		memcpy(mNormals.data(), geometry.normals.data(), mNumVertices * sizeof(Vector3));
	}

	//Create tangent attribute buffer
	if (mAttributes & VertexAttribute::Tangent)
	{
		mTangents.resize(mNumVertices);
		memcpy(mTangents.data(), geometry.tangents.data(), mNumVertices * sizeof(Vector3));
		mBinormals.resize(mNumVertices);
		memcpy(mBinormals.data(), geometry.binormals.data(), mNumVertices * sizeof(Vector3));
	}

	//Create UV attribute buffer
	if (mAttributes & VertexAttribute::Texcoord)
	{
		mTexcoords.resize(mNumVertices);
		memcpy(mTexcoords.data(), geometry.uvs.data(), mNumVertices * sizeof(Vector2));
	}

	CreateBuffers();
}

void Mesh::LoadFromFile(const std::string & path)
{
	//Open file
	FileStreamRead file(path);

	//Load mesh properties
	file.Read(mName);
	file.Read(mNumVertices);
	file.Read(mNumIndices);
	file.Read(mAttributes);

	//Load index data
	{
		mIndices.resize(mNumIndices);
		file.Read(mIndices.data(), mNumIndices * sizeof(uint32_t));
	}

	//Load position data
	{
		mPositions.resize(mNumVertices);
		file.Read(mPositions.data(), mNumVertices * sizeof(Vector3));
	}

	//Load normal data
	if (mAttributes & VertexAttribute::Normal) 
	{
		mNormals.resize(mNumVertices);
		file.Read(mNormals.data(), mNumVertices * sizeof(Vector3));
	}

	//Load tangent data
	if (mAttributes & VertexAttribute::Tangent) 
	{
		mTangents.resize(mNumVertices);
		file.Read(mTangents.data(), mNumVertices * sizeof(Vector3));

		mBinormals.resize(mNumVertices);
		file.Read(mBinormals.data(), mNumVertices * sizeof(Vector3));
	}

	//Load uv data
	if (mAttributes & VertexAttribute::Texcoord) 
	{
		mTexcoords.resize(mNumVertices);
		file.Read(mTexcoords.data(), mNumVertices * sizeof(Vector2));
	}

	//Load color data
	if (mAttributes & VertexAttribute::Color) 
	{
		mColors.resize(mNumVertices);
		file.Read(mColors.data(), mNumVertices * sizeof(Vector4));
	}

	//Load skin data
	if (mAttributes & VertexAttribute::Skin) 
	{
		mBlendIndices.resize(mNumVertices);
		file.Read(mBlendIndices.data(), mNumVertices * sizeof(Vector4));

		mBlendWeights.resize(mNumVertices);
		file.Read(mBlendWeights.data(), mNumVertices * sizeof(Vector4));

		mInverseBone.resize(file.Read<unsigned char>());
		for (size_t i = 0; i < mInverseBone.size(); i++) {
			file.Read(mInverseBone[i].BoneName);
			file.Read(mInverseBone[i].InverseTransform);
		}
	}

	//Create DirectX buffers
	CreateBuffers();
}

void Mesh::SaveToFile(const std::string & path)
{
	//Open or create file
	FileStreamWrite file(path);

	//Write mesh properties
	file.Write(mName);
	file.Write(mNumVertices);
	file.Write(mNumIndices);
	file.Write(mAttributes);

	//Write index data
	{
		file.Write(mIndices.data(), mNumIndices * sizeof(uint32_t));
	}

	//Write position data
	{
		file.Write(mPositions.data(), mNumVertices * sizeof(Vector3));
	}

	//Write normal data
	if (mAttributes & VertexAttribute::Normal)
	{
		file.Write(mNormals.data(), mNumVertices * sizeof(Vector3));
	}

	//Write tangent data
	if (mAttributes & VertexAttribute::Tangent)
	{
		file.Write(mTangents.data(), mNumVertices * sizeof(Vector3));
		file.Write(mBinormals.data(), mNumVertices * sizeof(Vector3));
	}

	//Write uv data
	if (mAttributes & VertexAttribute::Texcoord)
	{
		file.Write(mTexcoords.data(), mNumVertices * sizeof(Vector2));
	}

	//Write color data
	if (mAttributes & VertexAttribute::Color)
	{
		file.Write(mColors.data(), mNumVertices * sizeof(Vector4));
	}

	//Write skin data
	if (mAttributes & VertexAttribute::Skin)
	{
		file.Write(mBlendIndices.data(), mNumVertices * sizeof(Vector4));
		file.Write(mBlendWeights.data(), mNumVertices * sizeof(Vector4));

		file.Write((unsigned char)mInverseBone.size());
		for (size_t i = 0; i < mInverseBone.size(); i++) {
			file.Write(mInverseBone[i].BoneName);
			file.Write(mInverseBone[i].InverseTransform);
		}
	}
}

void Mesh::CreateBuffers()
{
	//Create index buffer.
	{
		mGraphics->CreateBuffer(
			mIndexBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_INDEX_BUFFER,
			mNumIndices * sizeof(uint32_t),
			mIndices.data());
	}

	//Create position attribute buffer
	{
		mGraphics->CreateBuffer(
			mPositionBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			mPositions.data());
	}

	//Create normal attribute buffer
	if (mAttributes & VertexAttribute::Normal)
	{
		mGraphics->CreateBuffer(
			mNormalBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			mNormals.data());
	}

	//Create tangent attribute buffer
	if (mAttributes & VertexAttribute::Tangent)
	{
		mGraphics->CreateBuffer(
			mTangentBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			mTangents.data());

		mGraphics->CreateBuffer(
			mBinormalBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector3),
			mBinormals.data());
	}

	//Create UV attribute buffer
	if (mAttributes & VertexAttribute::Texcoord)
	{
		mGraphics->CreateBuffer(
			mTexcoordBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector2),
			mTexcoords.data());
	}

	//Create color attribute buffer
	if (mAttributes & VertexAttribute::Color)
	{
		mGraphics->CreateBuffer(
			mColorBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector4),
			mColors.data());
	}

	//Create skin blending attribute buffer
	if (mAttributes & VertexAttribute::Skin)
	{
		mGraphics->CreateBuffer(
			mBlendIndicesBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector4),
			mBlendIndices.data());

		mGraphics->CreateBuffer(
			mBlendWeightsBuffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_VERTEX_BUFFER,
			mNumVertices * sizeof(Vector4),
			mBlendWeights.data());
	}
}

void Mesh::SetIndexCount(UINT count)
{
	mNumIndices = count;

	mIndices.resize(count);
}

void Mesh::SetVertexCount(UINT count)
{
	mNumVertices = count;

	if (mAttributes & Position) 
	{
		mPositions.resize(mNumVertices);
	}
	if (mAttributes & Texcoord) 
	{
		mTexcoords.resize(mNumVertices);
	}
	if (mAttributes & Normal) 
	{
		mNormals.resize(mNumVertices);
	}
	if (mAttributes & Tangent) 
	{
		mTangents.resize(mNumVertices);
		mBinormals.resize(mNumVertices);
	}
	if (mAttributes & Skin) 
	{
		mBlendIndices.resize(mNumVertices);
		mBlendWeights.resize(mNumVertices);
	}
}

void Mesh::SetAttributes(UINT attribute)
{
	mAttributes = attribute;

	SetVertexCount(mNumVertices);
}
