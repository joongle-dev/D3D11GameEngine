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
		std::vector<uint32_t> indices;
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector3> tangents;
		std::vector<Vector3> binormals;
		std::vector<Vector2> uvs;
		std::vector<Vector4> blendIndices;
		std::vector<Vector4> blendWeights;
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

private:
	void ExportMaterial();
	void ExportMesh();

	void ProcessFbxMaterial(const struct aiScene* scene, const std::string& filepath);
	void ProcessMaterial(const struct aiScene* scene);
	void ProcessMesh(const struct aiScene* scene);

private:
	class ResourceManager* m_resources;

	std::vector<MeshData> m_meshes;
	std::vector<MaterialData> m_materials;
};