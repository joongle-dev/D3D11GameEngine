#pragma once
#include "Resource.h"

class Mesh final : public Resource<Mesh>
{
public:
	enum VertexAttribute : uint32_t
	{
		POSITION = 1 << 0,
		TEXCOORD = 1 << 1,
		NORMAL   = 1 << 2,
		TANGENT  = 1 << 3,
		SKIN     = 1 << 4,
		COLOR    = 1 << 5,
	};

public:
	Mesh(class Context* context);
	~Mesh() = default;

	void Create(Geometry& geometry);

	void LoadFromFile(const std::string& path) override;

	const unsigned int GetIndexCount() const { return mNumIndices; }
	const unsigned int GetVertexCount() const { return mNumVertices; }

	ID3D11Buffer* GetIndices()   const { return mIndices.Get(); }
	ID3D11Buffer* GetPositions() const { return mPositions.Get(); }
	ID3D11Buffer* GetNormals()   const { return mNormals.Get(); }
	ID3D11Buffer* GetTangents()  const { return mTangents.Get(); }
	ID3D11Buffer* GetBinormals() const { return mBinormals.Get(); }
	ID3D11Buffer* GetTexcoords() const { return mTexcoords.Get(); }

private:
	class Graphics* mGraphics;

	uint32_t mNumVertices;
	uint32_t mNumIndices;
	uint32_t mAttributes;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPositions;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mNormals;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTangents;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBinormals;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTexcoords;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mColors;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBlendIndices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBlendWeights;
};