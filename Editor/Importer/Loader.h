#pragma once

class Importer final
{
	struct MeshData
	{
		unsigned int materialIndex;
		std::string name;
		uint32_t numVertices;
		uint32_t numIndices;
		uint32_t attribute;
		std::vector<uint32_t> mIndices;
		std::vector<Vector3> mPositions;
		std::vector<Vector3> mNormals;
		std::vector<Vector3> mTangents;
		std::vector<Vector3> mBinormals;
		std::vector<Vector2> mTexcoords;
		std::vector<Vector4> mBlendIndices;
		std::vector<Vector4> mBlendWeights;
		std::vector<Matrix> mBindposeInverses;
	};
	struct MaterialData
	{
		std::string name;
		std::string diffuseTex;
		std::string specularTex;
		std::string normalTex;
		
	};
public:
	Importer(class Context* context);
	~Importer() = default;

	void ImportModel(const std::string& filepath);
	void TestFunc(class Scene* scene, const std::string& filepath);

private:
	void ExportMaterial();
	void ExportMesh();

	void ProcessNodes(const struct aiScene* scene);

	void ProcessFbxMaterial(const struct aiScene* scene, const std::string& filepath);
	void ProcessMaterial(const struct aiScene* scene);
	void ProcessMesh(const struct aiScene* scene);

private:
	class Context* mContext;
	class ResourceManager* m_resources;

	std::vector<MeshData> mMeshes;
	std::vector<MaterialData> m_materials;
};