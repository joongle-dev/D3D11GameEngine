#pragma once

class Importer final
{
public:
	Importer(class Context* context);
	~Importer() = default;

	void ImportModel(const std::string& filepath);

private:
	void ProcessNodes(const struct aiScene* scene);

	void ProcessFbxMaterial(const struct aiScene* scene, const std::string& filepath);
	void ProcessAnimation(const struct aiScene* scene);
	void ProcessMaterial(const struct aiScene* scene);
	void ProcessMesh(const struct aiScene* scene);

private:
	class Context* mContext;
	class ResourceManager* mResources;

	std::vector<class Mesh*> mMeshes;
	std::vector<class Material*> mMaterials;
	std::vector<class Animation*> mAnimations;
};