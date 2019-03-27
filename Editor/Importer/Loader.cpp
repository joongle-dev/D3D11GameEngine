#include "stdafx.h"
#include "Loader.h"
#include <filesystem>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fbxsdk.h>

#pragma comment(lib, "libfbxsdk.lib")

Importer::Importer(Context * context) :
	m_context(context) 
{

}

void Importer::ImportModel(const std::string & filepath)
{
	Assimp::Importer importer;

	unsigned int flags = 
		aiProcess_Triangulate | 
		aiProcess_GenSmoothNormals | 
		aiProcess_FlipUVs | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_CalcTangentSpace;

	const aiScene* scene = importer.ReadFile(filepath, flags);
	
	if (scene) {
		fbxsdk::FbxManager* fbxManager = fbxsdk::FbxManager::Create();
		fbxsdk::FbxImporter* fbxImporter = fbxsdk::FbxImporter::Create(fbxManager, "");
		fbxsdk::FbxIOSettings* fbxIos = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxIos->SetBoolProp(IMP_FBX_TEXTURE, true);
		fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(fbxManager, "");
		fbxImporter->Initialize(filepath.c_str(), -1, fbxIos);
		fbxImporter->Import(fbxScene);

		ProcessMaterial(scene);
		ProcessMesh(scene);
	}
	else 
		printf("Error importing %s : %s", filepath, importer.GetErrorString());
}

void Importer::ProcessMaterial(const aiScene * scene)
{
	materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		MaterialData& materialData = materials[i];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			//material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), path);
			if (const aiTexture* texture = scene->GetEmbeddedTexture(path.data))
			{
				std::string filename = texture->mFilename.C_Str();
				aiTexel texel[256];
				memcpy(texel, texture->pcData, sizeof(aiTexel) * 256);
				int a = 0;
			}
			else
			{
				int a = 0;
			}
		}
	}
}

void Importer::ProcessMesh(const aiScene * scene)
{
	meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		MeshData& meshData = meshes[i];

		meshData.materialIndex = mesh->mMaterialIndex;

		if (mesh->HasPositions())
		{
			meshData.positions.assign(
				reinterpret_cast<Vector3*>(mesh->mVertices),
				reinterpret_cast<Vector3*>(mesh->mVertices) + mesh->mNumVertices);
		}
		if (mesh->HasNormals())
		{
			meshData.normals.assign(
				reinterpret_cast<Vector3*>(mesh->mNormals),
				reinterpret_cast<Vector3*>(mesh->mNormals) + mesh->mNumVertices);
		}
		if (mesh->HasTangentsAndBitangents())
		{
			meshData.tangents.assign(
				reinterpret_cast<Vector3*>(mesh->mTangents),
				reinterpret_cast<Vector3*>(mesh->mTangents) + mesh->mNumVertices);
			meshData.binormals.assign(
				reinterpret_cast<Vector3*>(mesh->mBitangents),
				reinterpret_cast<Vector3*>(mesh->mBitangents) + mesh->mNumVertices);
		}
		if (mesh->HasTextureCoords(0))
		{
			meshData.uvs.assign(
				reinterpret_cast<Vector2*>(mesh->mTextureCoords[0]),
				reinterpret_cast<Vector2*>(mesh->mTextureCoords[0]) + mesh->mNumVertices);
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
