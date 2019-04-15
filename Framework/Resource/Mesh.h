#pragma once
#include "Resource.h"

class Mesh final : public Resource<Mesh>
{
public:
	enum VertexAttribute : UINT
	{
		Position = 1 << 0,
		Texcoord = 1 << 1,
		Normal   = 1 << 2,
		Tangent  = 1 << 3,
		Skin     = 1 << 4,
		Color    = 1 << 5,
	};

	struct Bone
	{
		std::string BoneName;
		Matrix InverseTransform;
	};

public:
	Mesh(class Context* context);
	~Mesh() = default;

	void Create(Geometry& geometry);

	void LoadFromFile(const std::string& path) override;
	void SaveToFile(const std::string& path) override;

	void CreateBuffers();

	void SetIndexCount(UINT count);
	void SetVertexCount(UINT count);
	void SetAttributes(UINT attribute);

	const UINT GetVertexCount() const { return mNumVertices; }
	const UINT GetIndexCount()  const { return mNumIndices; }
	const UINT GetAttributes()  const { return mAttributes; }

	uint32_t* GetIndexData()       { return mIndices.data(); }
	Vector3*  GetPositionData()    { return mPositions.data(); }
	Vector3*  GetNormalData()      { return mNormals.data(); }
	Vector3*  GetTangentData()     { return mTangents.data(); }
	Vector3*  GetBinormalData()    { return mBinormals.data(); }
	Vector2*  GetTexcoordData()    { return mTexcoords.data(); }
	Vector4*  GetBlendIndexData()  { return mBlendIndices.data(); }
	Vector4*  GetBlendWeightData() { return mBlendIndices.data(); }

	Bone* GetBoneData() { return mInverseBone.data(); }
	UINT GetBoneCount() { return mInverseBone.size(); }
	void SetBoneCount(UINT count) { mInverseBone.resize(count); }

	ID3D11Buffer* GetIndexBuffer()    const { return mIndexBuffer.Get(); }
	ID3D11Buffer* GetPositionBuffer() const { return mPositionBuffer.Get(); }
	ID3D11Buffer* GetNormalBuffer()   const { return mNormalBuffer.Get(); }
	ID3D11Buffer* GetTangentBuffer()  const { return mTangentBuffer.Get(); }
	ID3D11Buffer* GetBinormalBuffer() const { return mBinormalBuffer.Get(); }
	ID3D11Buffer* GetTexcoordBuffer() const { return mTexcoordBuffer.Get(); }

private:
	class Graphics* mGraphics;

	uint32_t mNumVertices;
	uint32_t mNumIndices;
	uint32_t mAttributes;

	std::vector<uint32_t> mIndices;
	std::vector<Vector3> mPositions;
	std::vector<Vector3> mNormals;
	std::vector<Vector3> mTangents;
	std::vector<Vector3> mBinormals;
	std::vector<Vector2> mTexcoords;
	std::vector<Vector4> mColors;
	std::vector<Vector4> mBlendIndices;
	std::vector<Vector4> mBlendWeights;
	std::vector<Bone> mInverseBone;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPositionBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mNormalBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTangentBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBinormalBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTexcoordBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mColorBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBlendIndicesBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBlendWeightsBuffer;
};