#pragma once

#define FBXSDK_NEW_API
#define FBXSDK_SHARED 
#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")

class FbxLoader
{
	struct FbxSkinData
	{
	private:
		typedef std::pair<uint32_t, float> FbxVertexBlend;
		typedef std::vector<FbxVertexBlend> FbxVertexBlends;
		FbxVertexBlends blends;
	public:
		void Insert(uint32_t index, float weight);
		DirectX::XMFLOAT4 Indices();
		DirectX::XMFLOAT4 Weights();
	};
	struct FbxMaterial
	{
		std::string name;
		std::string diffuseTexture;
		std::string specularTexture;
		std::string normalTexture;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 specularColor;
		float specularPower;
	};
	struct FbxBoneData
	{
		std::string name;
		DirectX::XMFLOAT4X4 global;
		int index;
		int parentIndex;
	};
	struct FbxMeshData
	{
		std::string name;
		uint32_t numVertices;
		uint32_t numIndices;
		uint32_t attribute;
		std::vector<uint32_t> indices;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<DirectX::XMFLOAT4> blendIndices;
		std::vector<DirectX::XMFLOAT4> blendWeights;
		void SaveAsFile();
	};
	struct FbxAnimData
	{
		std::string name;
	};
public:
	FbxLoader();
	~FbxLoader();

	void Load(std::string filename);

	void ProcessBoneData(fbxsdk::FbxNode* node, int index, int parent);
	void ProcessMeshData(fbxsdk::FbxNode* node);

	void ProcessMaterials();
	void ProcessAnimationData();
	void ProcessKeyFrameData(fbxsdk::FbxNode* node, int start, int end, int index);

private:
	std::vector<FbxSkinData> GetSkinData(fbxsdk::FbxMesh* mesh);
	int GetBoneIndex(std::string name);
	int GetMaterialIndex(std::string name);

	fbxsdk::FbxManager* manager;
	fbxsdk::FbxIOSettings* ios;
	fbxsdk::FbxScene* scene;
	fbxsdk::FbxImporter* importer;
	fbxsdk::FbxGeometryConverter* converter;

	std::vector<FbxMaterial> materials;
	std::vector<FbxBoneData> bones;
	std::vector<FbxMeshData> meshes;
	std::vector<FbxAnimData> animations;
};

