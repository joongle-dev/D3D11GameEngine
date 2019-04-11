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



Importer::Importer(Context * context) :
	mContext(context)
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

	const aiScene* pModelScene = importer.ReadFile(filepath, flags);

	ProcessNodes(pModelScene);
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

	for (auto mesh : mMeshes)
	{
		FileStreamWrite file;
		file.Open(directory + mesh.name + ".mesh");

		file.Write(mesh.name);
		file.Write(mesh.numVertices);
		file.Write(mesh.numIndices);
		file.Write(mesh.attribute);
		file.Write(mesh.mIndices.data(), mesh.numIndices * sizeof(uint32_t));
		file.Write(mesh.mPositions.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.mNormals.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.mTangents.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.mBinormals.data(), mesh.numVertices * sizeof(Vector3));
		file.Write(mesh.mTexcoords.data(), mesh.numVertices * sizeof(Vector2));
	}
}

void Importer::ProcessNodes(const aiScene * pAiScene)
{
	printf("%d\n", pAiScene->mAnimations[0]->mNumChannels);
	for (unsigned int i = 0; i < pAiScene->mNumAnimations; i++)
	{
		aiAnimation* anim = pAiScene->mAnimations[i];
		for (unsigned int j = 0; j < anim->mNumChannels; j++)
			printf("%s\n", anim->mChannels[j]->mNodeName.C_Str());
	}
	printf("\n");

	Scene* pGameScene = mContext->GetSubsystem<SceneManager>()->GetCurrentScene();

	auto RecursiveFunc = [pGameScene, pAiScene](Transform* pParentTransform, aiNode* pAiNode, const auto& lambda)->void
	{
		GameObject* pNodeObject = pGameScene->Instantiate();
		pNodeObject->SetName(pAiNode->mName.C_Str());

		if (pAiNode->mNumMeshes)
		{
			MeshRenderer* pMeshRenderer = pNodeObject->AddComponent<MeshRenderer>();
			aiMesh* pModelMesh = pAiScene->mMeshes[pAiNode->mMeshes[0]];
			printf("%d\n", pModelMesh->mNumBones);
			for (unsigned int i = 0; i < pModelMesh->mNumBones; i++)
				printf("%s\n", pModelMesh->mBones[i]->mName.C_Str());
			printf("\n");
		}

		Transform* transform = pNodeObject->GetComponent<Transform>();
		transform->SetParent(pParentTransform);
		for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
			lambda(transform, pAiNode->mChildren[i], lambda);
	};
	RecursiveFunc(pGameScene->GetRoot(), pAiScene->mRootNode, RecursiveFunc);
}

void Importer::ProcessFbxMaterial(const aiScene * pAiScene, const std::string & filepath)
{
	using namespace fbxsdk;

	FbxManager* fbxManager = FbxManager::Create();
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	FbxIOSettings* fbxIos = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxIos->SetBoolProp(IMP_FBX_TEXTURE, true);
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	fbxImporter->Initialize(filepath.c_str(), -1, fbxIos);
	fbxImporter->Import(fbxScene);

	for (unsigned int i = 0; i < pAiScene->mNumMaterials; i++)
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

void Importer::ProcessMesh(const aiScene * pAiScene)
{
	mMeshes.resize(pAiScene->mNumMeshes);

	for (unsigned int i = 0; i < pAiScene->mNumMeshes; i++)
	{
		const aiMesh* pAiMesh = pAiScene->mMeshes[i];
		MeshData& meshData = mMeshes[i];

		meshData.name = pAiMesh->mName.C_Str();
		meshData.materialIndex = pAiMesh->mMaterialIndex;
		meshData.numVertices = pAiMesh->mNumVertices;
		meshData.numIndices = pAiMesh->mNumFaces * 3;
		meshData.attribute = 0;

		if (pAiMesh->HasPositions())
		{
			meshData.mPositions.assign(
				reinterpret_cast<Vector3*>(pAiMesh->mVertices),
				reinterpret_cast<Vector3*>(pAiMesh->mVertices) + pAiMesh->mNumVertices);
		}
		if (pAiMesh->HasNormals())
		{
			meshData.attribute |= Mesh::NORMAL;
			meshData.mNormals.assign(
				reinterpret_cast<Vector3*>(pAiMesh->mNormals),
				reinterpret_cast<Vector3*>(pAiMesh->mNormals) + pAiMesh->mNumVertices);
		}
		if (pAiMesh->HasTangentsAndBitangents())
		{
			meshData.attribute |= Mesh::TANGENT;
			meshData.mTangents.assign(
				reinterpret_cast<Vector3*>(pAiMesh->mTangents),
				reinterpret_cast<Vector3*>(pAiMesh->mTangents) + pAiMesh->mNumVertices);
			meshData.mBinormals.assign(
				reinterpret_cast<Vector3*>(pAiMesh->mBitangents),
				reinterpret_cast<Vector3*>(pAiMesh->mBitangents) + pAiMesh->mNumVertices);
		}
		if (pAiMesh->HasTextureCoords(0))
		{
			meshData.attribute |= Mesh::TEXCOORD;
			meshData.mTexcoords.resize(pAiMesh->mNumVertices);
			for (unsigned int j = 0; j < pAiMesh->mNumVertices; j++) {
				meshData.mTexcoords[j].x = pAiMesh->mTextureCoords[0][j].x;
				meshData.mTexcoords[j].y = pAiMesh->mTextureCoords[0][j].y;
			}
		}
		if (pAiMesh->HasBones())
		{
			meshData.attribute != Mesh::SKIN;
			meshData.mBlendIndices.resize(pAiMesh->mNumVertices, Vector4());
			meshData.mBlendWeights.resize(pAiMesh->mNumVertices, Vector4());
			for (unsigned int j = 0; j < pAiMesh->mNumBones; j++) {
				aiBone* pMeshBone = pAiMesh->mBones[j];
				for (unsigned int k = 0; k < pMeshBone->mNumWeights; k++) {
					aiVertexWeight& Weight = pMeshBone->mWeights[k];
					for (unsigned int l = 0; l < 4; l++) {
						float* pBlendIndex = reinterpret_cast<float*>(&meshData.mBlendIndices[Weight.mVertexId]) + l;
						float* pBlendWeight = reinterpret_cast<float*>(&meshData.mBlendWeights[Weight.mVertexId]) + l;
						if (*pBlendWeight == 0) {
							*pBlendIndex = j;
							*pBlendWeight = Weight.mWeight;
							break;
						}
					}
				}
				//meshData.mBindposeInverses[j] = pMeshBone->mOffsetMatrix;
			}
		}

		meshData.mIndices.resize(pAiMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pAiMesh->mNumFaces; i++)
		{
			assert(pAiMesh->mFaces[i].mNumIndices == 3);
			meshData.mIndices[i * 3 + 0] = pAiMesh->mFaces[i].mIndices[0];
			meshData.mIndices[i * 3 + 1] = pAiMesh->mFaces[i].mIndices[1];
			meshData.mIndices[i * 3 + 2] = pAiMesh->mFaces[i].mIndices[2];
		}
	}
}

