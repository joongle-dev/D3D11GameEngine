#include "stdafx.h"
#include "Loader.h"
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Vector2 AiToVector2(const aiVector3D& aiVec)
{
	return Vector2(
		static_cast<float>(aiVec.x),
		static_cast<float>(aiVec.y)
	);
}

Vector3 AiToVector3(const aiVector3D& aiVec)
{
	return Vector3(
		static_cast<float>(aiVec.x), 
		static_cast<float>(aiVec.y), 
		static_cast<float>(aiVec.z)
	);
}

Matrix AiToMatrix(const aiMatrix4x4& aiMat)
{
	return Matrix(
		aiMat.a1, aiMat.a2, aiMat.a3, aiMat.a4,
		aiMat.b1, aiMat.b2, aiMat.b3, aiMat.c4,
		aiMat.c1, aiMat.c2, aiMat.c3, aiMat.c4,
		aiMat.d1, aiMat.d2, aiMat.d3, aiMat.d4
	);
}

Importer::Importer(Context * context) :
	mContext(context)
{
	mResources = context->GetSubsystem<ResourceManager>();
}

void Importer::ImportModel(const std::string & filepath)
{
	Assimp::Importer AiImporter;
	AiImporter.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	AiImporter.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	//AiImporter.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.1f);
	AiImporter.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	AiImporter.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000);
	AiImporter.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	AiImporter.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	AiImporter.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

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
		//aiProcess_GlobalScale |
		aiProcess_ConvertToLeftHanded;

	if (const aiScene* pAiScene = AiImporter.ReadFile(filepath, flags))
	{
		ProcessMesh(pAiScene);
		ProcessMaterial(pAiScene);
		ProcessAnimation(pAiScene);
		ProcessNodes(pAiScene);

		double temp;
		if (pAiScene->mMetaData->Get("UnitScaleFactor", temp))
			printf("%f", temp);
		else
			printf("Failed to retrieve unit scale factor");
	}
	else
	{
		printf("%s", AiImporter.GetErrorString());
		return;
	}
}

void Importer::ProcessNodes(const aiScene * pAiScene)
{
	Scene* pGameScene = mContext->GetSubsystem<SceneManager>()->GetCurrentScene();

	auto RecursiveFunc = [pGameScene, pAiScene, this](Transform* pParentTransform, aiNode* pAiNode, const auto& lambda)->void
	{
		GameObject* pNodeObject = pGameScene->Instantiate();
		pNodeObject->SetName(pAiNode->mName.C_Str());

		if (pAiNode->mNumMeshes)
		{
			MeshRenderer* pMeshRenderer = pNodeObject->AddComponent<MeshRenderer>();
			pMeshRenderer->SetMesh(mMeshes[pAiNode->mMeshes[0]]);
			pMeshRenderer->SetMaterial(mMaterials[pAiScene->mMeshes[pAiNode->mMeshes[0]]->mMaterialIndex]);
		}

		Transform* transform = pNodeObject->GetComponent<Transform>();
		transform->SetParent(pParentTransform);
		for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
			lambda(transform, pAiNode->mChildren[i], lambda);
	};
	RecursiveFunc(pGameScene->GetRoot(), pAiScene->mRootNode, RecursiveFunc);
}

void Importer::ProcessAnimation(const aiScene * pAiScene)
{
	for (unsigned int i = 0; i < pAiScene->mNumAnimations; i++)
	{
		aiAnimation* pAiAnimation = pAiScene->mAnimations[i];
		printf("%s", pAiAnimation->mName.C_Str());
	}
	
}

void Importer::ProcessMaterial(const aiScene * pAiScene)
{
	ResourceManager* pResourceManager = mContext->GetSubsystem<ResourceManager>();

	mMaterials.resize(pAiScene->mNumMaterials);
	for (unsigned int i = 0; i < pAiScene->mNumMaterials; i++)
	{
		const aiMaterial* pAiMaterial = pAiScene->mMaterials[i];
		Material* pMaterial = mMaterials[i] = new Material(mContext);

		aiString stringbuf;
		
		if (pAiMaterial->Get(AI_MATKEY_NAME, stringbuf) == AI_SUCCESS)
			pMaterial->SetName(stringbuf.C_Str());

		auto GetMaterialTexture = [pAiScene, pAiMaterial, pResourceManager](aiTextureType type)->Texture*
		{
			aiString aipath;
			if (pAiMaterial->GetTexture(type, 0, &aipath) == AI_SUCCESS)
			{
				std::string texpath = aipath.C_Str();
				texpath = "../Assets/Texture" + texpath.substr(texpath.find_last_of("/\\"));
				if (const aiTexture* pEmbeddedTexture = pAiScene->GetEmbeddedTexture(aipath.data))
				{
					FileStreamWrite w(texpath);

					if (pEmbeddedTexture->mHeight)
						w.Write(pEmbeddedTexture->pcData, pEmbeddedTexture->mWidth * pEmbeddedTexture->mHeight);
					else
						w.Write(pEmbeddedTexture->pcData, pEmbeddedTexture->mWidth);
				}
				return pResourceManager->Load<Texture>(texpath);
			}
			return nullptr;
		};

		pMaterial->SetTexture(Material::Albedo,   GetMaterialTexture(aiTextureType_DIFFUSE));
		pMaterial->SetTexture(Material::Specular, GetMaterialTexture(aiTextureType_SPECULAR));
		pMaterial->SetTexture(Material::Normal,   GetMaterialTexture(aiTextureType_NORMALS));
		pMaterial->SetTexture(Material::Height,   GetMaterialTexture(aiTextureType_HEIGHT));
		pMaterial->SetTexture(Material::Roughness,GetMaterialTexture(aiTextureType_SHININESS));
		pMaterial->SetTexture(Material::Emissive, GetMaterialTexture(aiTextureType_EMISSIVE));
		pMaterial->SetTexture(Material::Ambient,  GetMaterialTexture(aiTextureType_AMBIENT));
	}
}

void Importer::ProcessMesh(const aiScene * pAiScene)
{
	mMeshes.resize(pAiScene->mNumMeshes);
	for (unsigned int i = 0; i < pAiScene->mNumMeshes; i++)
	{
		const aiMesh* pAiMesh = pAiScene->mMeshes[i];

		Mesh* pMesh = mMeshes[i] = new Mesh(mContext);

		unsigned int attribute = 0;
		attribute |= pAiMesh->HasPositions()             ? Mesh::Position : 0;
		attribute |= pAiMesh->HasNormals()               ? Mesh::Normal   : 0;
		attribute |= pAiMesh->HasTangentsAndBitangents() ? Mesh::Tangent  : 0;
		attribute |= pAiMesh->HasTextureCoords(0)        ? Mesh::Texcoord : 0;
		attribute |= pAiMesh->HasBones()                 ? Mesh::Skin     : 0;

		pMesh->SetAttributes(attribute);
		pMesh->SetName(pAiMesh->mName.C_Str());
		pMesh->SetIndexCount(pAiMesh->mNumFaces * 3);
		pMesh->SetVertexCount(pAiMesh->mNumVertices);

		if (attribute & Mesh::Position)
		{
			//memcpy(pMesh->mPositions.get(), pAiMesh->mVertices, pAiMesh->mNumVertices * sizeof(Vector3));
			for (unsigned int j = 0; j < pAiMesh->mNumVertices; j++)
				pMesh->GetPositionData()[j] = AiToVector3(pAiMesh->mVertices[j]) * 0.01f;
		}
		if (attribute & Mesh::Normal)
		{
			memcpy(pMesh->GetNormalData(), pAiMesh->mNormals, pAiMesh->mNumVertices * sizeof(Vector3));
		}
		if (attribute & Mesh::Tangent)
		{
			memcpy(pMesh->GetTangentData(), pAiMesh->mTangents, pAiMesh->mNumVertices * sizeof(Vector3));
			memcpy(pMesh->GetBinormalData(), pAiMesh->mBitangents, pAiMesh->mNumVertices * sizeof(Vector3));
		}
		if (attribute & Mesh::Texcoord)
		{
			for (unsigned int j = 0; j < pAiMesh->mNumVertices; j++)
				pMesh->GetTexcoordData()[j] = AiToVector2(pAiMesh->mTextureCoords[0][j]);
		}
		if (attribute & Mesh::Skin)
		{
			pMesh->SetBoneCount(pAiMesh->mNumBones);
			for (unsigned int j = 0; j < pAiMesh->mNumBones; j++) {
				const aiBone* pMeshBone = pAiMesh->mBones[j];
				for (unsigned int k = 0; k < pMeshBone->mNumWeights; k++) {
					const aiVertexWeight& aiWeight = pMeshBone->mWeights[k];
					float* pBlendIndices = &pMesh->GetBlendIndexData()[aiWeight.mVertexId].x;
					float* pBlendWeights = &pMesh->GetBlendWeightData()[aiWeight.mVertexId].x;
					for (unsigned int l = 0; l < 4; l++)
						if (pBlendWeights[l] == 0) {
							pBlendIndices[l] = static_cast<float>(j);
							pBlendWeights[l] = aiWeight.mWeight;
							break;
						}
				}
				pMesh->GetBoneData()[j].BoneName = pMeshBone->mName.C_Str();
				pMesh->GetBoneData()[j].InverseTransform = AiToMatrix(pMeshBone->mOffsetMatrix);
				printf("%s\n", pMeshBone->mName.C_Str());
			}
			printf("\n");
		}

		auto* pIndexData = pMesh->GetIndexData();
		for (unsigned int i = 0; i < pAiMesh->mNumFaces; i++)
		{
			pIndexData[i * 3 + 0] = pAiMesh->mFaces[i].mIndices[0];
			pIndexData[i * 3 + 1] = pAiMesh->mFaces[i].mIndices[1];
			pIndexData[i * 3 + 2] = pAiMesh->mFaces[i].mIndices[2];
		}

		pMesh->CreateBuffers();
	}
}

