#include "stdafx.h"
#include "Loader.h"
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

#define FBXSDK_NEW_API
#define FBXSDK_SHARED 
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

Importer::Importer(Context * context)
{
	m_resources = context->GetSubsystem<ResourceManager>();
}

void Importer::ImportModel(const std::string & filepath)
{
	Assimp::Importer importer;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

	//Get Version
	const int major    = aiGetVersionMajor();
	const int minor    = aiGetVersionMinor();
	const int revision = aiGetVersionRevision();

	unsigned int flags =
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = importer.ReadFile(filepath, flags);
	
	if (scene) {
		ProcessMaterial(scene);
		if (filepath.rfind("fbx") != std::string::npos)
			ProcessFbxMaterial(scene, filepath);
		ProcessMesh(scene);
	}
	else 
		printf("Error importing %s : %s", filepath, importer.GetErrorString());

	ExportMaterial();
	ExportMesh();

	scene->mRootNode;

}

void Importer::TestFunc(Scene * scene, const std::string & filepath)
{
	Assimp::Importer importer;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

	//Get Version
	const int major = aiGetVersionMajor();
	const int minor = aiGetVersionMinor();
	const int revision = aiGetVersionRevision();

	unsigned int flags =
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_ConvertToLeftHanded;

	const aiScene* modelscene = importer.ReadFile(filepath, flags);

	int test = 0;
	if (modelscene->mNumAnimations)
	{
		for (unsigned int i = 0; i < modelscene->mAnimations[0]->mNumChannels; i++)
			printf("%s\n", modelscene->mAnimations[0]->mChannels[i]->mNodeName.C_Str());
	}
	printf("\n");
	auto RecursiveFunc = [modelscene, scene, &test](Transform* parent, aiNode* node, const auto& lambda)->void
	{
		test++;
		GameObject* object = scene->Instantiate();
		object->SetName(node->mName.C_Str());
		if (node->mNumMeshes)
		{
			MeshRenderer* meshrender = object->AddComponent<MeshRenderer>();
			aiMesh* mesh = modelscene->mMeshes[node->mMeshes[0]];
			for (unsigned int i = 0; i < mesh->mNumBones; i++)
				printf("%s\n", mesh->mBones[i]->mName.C_Str());
		}
		Transform* transform = object->GetComponent<Transform>();
		transform->SetParent(parent);
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			lambda(transform, node->mChildren[i], lambda);
	};
	RecursiveFunc(scene->GetRoot(), modelscene->mRootNode, RecursiveFunc);
	printf("%d\n", test);
}

void Importer::ExportMaterial()
{
	std::string directory = m_resources->GetDirectory<Material>();

	for (auto material : m_materials)
	{
		FileStreamWrite file;
		file.Open(directory + material.name + ".mat");
		
		file.Write(material.name);

		std::string stringbuf;

		stringbuf = material.diffuseTex.empty() ? "" : material.diffuseTex.substr(material.diffuseTex.find_last_of("\\/"));
		if (!material.diffuseTex.empty())
			std::filesystem::copy_file(material.diffuseTex, m_resources->GetDirectory<Texture>() + stringbuf, std::filesystem::copy_options::overwrite_existing);
		file.Write(stringbuf);

		stringbuf = material.specularTex.empty() ? "" : material.specularTex.substr(material.specularTex.find_last_of("\\/"));
		if (!material.specularTex.empty())
			std::filesystem::copy_file(material.specularTex, m_resources->GetDirectory<Texture>() + stringbuf, std::filesystem::copy_options::overwrite_existing);
		file.Write(stringbuf);

		stringbuf = material.normalTex.empty() ? "" : material.normalTex.substr(material.normalTex.find_last_of("\\/"));
		if (!material.normalTex.empty())
			std::filesystem::copy_file(material.normalTex, m_resources->GetDirectory<Texture>() + stringbuf, std::filesystem::copy_options::overwrite_existing);
		file.Write(stringbuf);
	}
}

void Importer::ExportMesh()
{
	std::string directory = m_resources->GetDirectory<Mesh>();

	for (auto mesh : m_meshes)
	{
		FileStreamWrite file;
		file.Open(directory + mesh.name + ".mesh");

		file.Write(mesh.name);
		file.Write(mesh.numVertices);
		file.Write(mesh.numIndices);
		file.Write(mesh.attribute);
		file.Write(mesh.indices.data(), mesh.numIndices * sizeof(uint32_t));
		file.Write(mesh.positions.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.normals.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.tangents.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.binormals.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.texcoords.data(), mesh.numVertices * sizeof(Vector2));
	}
}
void Importer::ProcessFbxMaterial(const aiScene * scene, const std::string & filepath)
{
	using namespace fbxsdk;

	FbxManager* fbxManager = FbxManager::Create();
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	FbxIOSettings* fbxIos = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxIos->SetBoolProp(IMP_FBX_TEXTURE, true);
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	fbxImporter->Initialize(filepath.c_str(), -1, fbxIos);
	fbxImporter->Import(fbxScene);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = fbxScene->GetMaterial(i);
		FbxProperty property;

		property = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (property.IsValid() == true && property.GetSrcObjectCount() > 0)
			if (FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>())
				m_materials[i].diffuseTex = std::string(texture->GetFileName());

		property = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		if (property.IsValid() == true && property.GetSrcObjectCount() > 0)
			if (FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>())
				m_materials[i].specularTex = std::string(texture->GetFileName());

		property = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (property.IsValid() == true && property.GetSrcObjectCount() > 0)
			if (FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>())
				m_materials[i].normalTex = std::string(texture->GetFileName());
	}
}

void Importer::ProcessMaterial(const aiScene * scene)
{
	m_materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		MaterialData& materialData = m_materials[i];

		aiString stringbuf;
		if (material->Get(AI_MATKEY_NAME, stringbuf) == AI_SUCCESS)
			materialData.name = stringbuf.C_Str();
		if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), stringbuf) == AI_SUCCESS)
			materialData.diffuseTex = stringbuf.C_Str();
		if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), stringbuf) == AI_SUCCESS)
			materialData.specularTex = stringbuf.C_Str();
		if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), stringbuf) == AI_SUCCESS)
			materialData.normalTex = stringbuf.C_Str();
	}
}

void Importer::ProcessMesh(const aiScene * scene)
{
	m_meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		MeshData& meshData = m_meshes[i];

		meshData.name = mesh->mName.C_Str();
		meshData.materialIndex = mesh->mMaterialIndex;
		meshData.numVertices = mesh->mNumVertices;
		meshData.numIndices = mesh->mNumFaces * 3;
		meshData.attribute = 0;

		if (mesh->HasPositions())
		{
			meshData.positions.assign(
				reinterpret_cast<Vector3*>(mesh->mVertices),
				reinterpret_cast<Vector3*>(mesh->mVertices) + mesh->mNumVertices);
		}
		if (mesh->HasNormals())
		{
			meshData.attribute |= Mesh::NORMAL;
			meshData.normals.assign(
				reinterpret_cast<Vector3*>(mesh->mNormals),
				reinterpret_cast<Vector3*>(mesh->mNormals) + mesh->mNumVertices);
		}
		if (mesh->HasTangentsAndBitangents())
		{
			meshData.attribute |= Mesh::TANGENT;
			meshData.tangents.assign(
				reinterpret_cast<Vector3*>(mesh->mTangents),
				reinterpret_cast<Vector3*>(mesh->mTangents) + mesh->mNumVertices);
			meshData.binormals.assign(
				reinterpret_cast<Vector3*>(mesh->mBitangents),
				reinterpret_cast<Vector3*>(mesh->mBitangents) + mesh->mNumVertices);
		}
		if (mesh->HasTextureCoords(0))
		{
			meshData.attribute |= Mesh::TEXCOORD;
			meshData.texcoords.resize(mesh->mNumVertices);
			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
				meshData.texcoords[j].x = mesh->mTextureCoords[0][j].x;
				meshData.texcoords[j].y = mesh->mTextureCoords[0][j].y;
			}
		}
		if (mesh->HasBones())
		{
			mesh->mBones;
		}

		meshData.indices.resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			assert(mesh->mFaces[i].mNumIndices == 3);
			meshData.indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
			meshData.indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			meshData.indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}
	}
}

