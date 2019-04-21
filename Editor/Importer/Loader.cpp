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

Quaternion AiToQuaternion(const aiQuaternion& aiQuat)
{
	return Quaternion(
		static_cast<float>(aiQuat.x),
		static_cast<float>(aiQuat.y),
		static_cast<float>(aiQuat.z),
		static_cast<float>(aiQuat.w)
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
	Transform* temp = nullptr;
	auto RecursiveFunc = [pGameScene, pAiScene, this, &temp](Transform* pParentTransform, aiNode* pAiNode, const auto& lambda)->void
	{
		GameObject* pNodeObject = pGameScene->Instantiate();
		pNodeObject->SetName(pAiNode->mName.C_Str());

		if (pAiNode == pAiScene->mRootNode)
		{
			Animator* pAnimator = pNodeObject->AddComponent<Animator>();
			pAnimator->SetAnimation(mAnimations[0]);
		}

		if (pAiNode->mNumMeshes)
		{
			//MeshRenderer* pMeshRenderer = pNodeObject->AddComponent<MeshRenderer>();
			SkinnedMeshRenderer* pMeshRenderer = pNodeObject->AddComponent<SkinnedMeshRenderer>();
			pMeshRenderer->SetRootTransform(temp);
			pMeshRenderer->SetMesh(mMeshes[pAiNode->mMeshes[0]]);
			pMeshRenderer->SetMaterial(mMaterials[pAiScene->mMeshes[pAiNode->mMeshes[0]]->mMaterialIndex]);
		}

		Vector3 scale;
		Quaternion rotation;
		Vector3 position;
		Matrix nodeTransform = AiToMatrix(pAiNode->mTransformation);
		nodeTransform.Decompose(scale, rotation, position);

		Transform* transform = pNodeObject->GetComponent<Transform>();
		transform->SetParent(pParentTransform);
		transform->SetLocalScale(scale);
		transform->SetLocalRotation(rotation);
		transform->SetLocalPosition(scale);
		if (pAiNode == pAiScene->mRootNode)
			temp = transform;
		for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
			lambda(transform, pAiNode->mChildren[i], lambda);
	};
	RecursiveFunc(pGameScene->GetRoot(), pAiScene->mRootNode, RecursiveFunc);
}

void Importer::ProcessAnimation(const aiScene * pAiScene)
{
	mAnimations.resize(pAiScene->mNumAnimations);
	for (unsigned int idxAnim = 0; idxAnim < pAiScene->mNumAnimations; idxAnim++)
	{
		const aiAnimation* pAiAnimation = pAiScene->mAnimations[idxAnim];
		Animation* pAnimation = mAnimations[idxAnim] = new Animation(mContext);
		pAnimation->SetFramerate(static_cast<float>(pAiAnimation->mTicksPerSecond));
		pAnimation->SetDuration(static_cast<float>(pAiAnimation->mDuration));
		for (unsigned int idxChannel = 0; idxChannel < pAiAnimation->mNumChannels; idxChannel++)
		{
			const aiNodeAnim* pAiBoneAnim = pAiAnimation->mChannels[idxChannel];
			Animation::BoneAnimation& pBoneAnim = pAnimation->AddChannel(pAiBoneAnim->mNodeName.C_Str());
			
			unsigned int idxPositionKey = 0;
			unsigned int idxRotationKey = 0;
			unsigned int idxScaleKey    = 0;
			const aiVectorKey* pPositionKeys = pAiBoneAnim->mPositionKeys;
			const aiQuatKey*   pRotationKeys = pAiBoneAnim->mRotationKeys;
			const aiVectorKey* pScaleKeys    = pAiBoneAnim->mScalingKeys;
			for (double currTime = 0.0f; currTime < pAiAnimation->mDuration; currTime += pAiAnimation->mTicksPerSecond)
			{
				Animation::BoneKeyframe& keyframe = pBoneAnim.emplace_back(Animation::BoneKeyframe());
				keyframe.mTime = static_cast<float>(currTime);

				while (idxPositionKey + 1 < pAiBoneAnim->mNumPositionKeys && currTime > pPositionKeys[idxPositionKey + 1].mTime)
					idxPositionKey++;
				Vector3 vKeyPosition0 = AiToVector3(pPositionKeys[idxPositionKey].mValue);
				Vector3 vKeyPosition1 = AiToVector3(pPositionKeys[idxPositionKey + 1].mValue);
				double fKeyTime0 = pPositionKeys[idxPositionKey].mTime;
				double fKeyTime1 = pPositionKeys[idxPositionKey + 1].mTime;
				double fBlend = std::max((currTime - fKeyTime0) / (fKeyTime1 - fKeyTime0), 0.0);
				keyframe.mPosition = Vector3::Lerp(vKeyPosition0, vKeyPosition1, static_cast<float>(fBlend));
				
				while (idxRotationKey + 1 < pAiBoneAnim->mNumRotationKeys && currTime > pRotationKeys[idxRotationKey + 1].mTime)
					idxRotationKey++;
				Quaternion vKeyRotation0 = AiToQuaternion(pRotationKeys[idxRotationKey].mValue);
				Quaternion vKeyRotation1 = AiToQuaternion(pRotationKeys[idxRotationKey + 1].mValue);
				fKeyTime0 = pRotationKeys[idxRotationKey].mTime;
				fKeyTime0 = pRotationKeys[idxRotationKey + 1].mTime;
				fBlend = std::max((currTime - fKeyTime0) / (fKeyTime1 - fKeyTime0), 0.0);
				keyframe.mRotation = Quaternion::Lerp(vKeyRotation0, vKeyRotation1, static_cast<float>(fBlend));

				while (idxScaleKey + 1 < pAiBoneAnim->mNumScalingKeys && currTime > pScaleKeys[idxScaleKey + 1].mTime)
					idxScaleKey++;
				Vector3 vKeyScale0 = AiToVector3(pScaleKeys[idxPositionKey].mValue);
				Vector3 vKeyScale1 = AiToVector3(pScaleKeys[idxPositionKey + 1].mValue);
				fKeyTime0 = pScaleKeys[idxPositionKey].mTime;
				fKeyTime1 = pScaleKeys[idxPositionKey + 1].mTime;
				fBlend = std::max((currTime - fKeyTime0) / (fKeyTime1 - fKeyTime0), 0.0);
				keyframe.mScale = Vector3::Lerp(vKeyScale0, vKeyScale1, static_cast<float>(fBlend));
			}
		}
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
			memcpy(pMesh->GetPositionData(), pAiMesh->mVertices, pAiMesh->mNumVertices * sizeof(Vector3));
			//for (unsigned int j = 0; j < pAiMesh->mNumVertices; j++)
			//	pMesh->GetPositionData()[j] = AiToVector3(pAiMesh->mVertices[j]) * 0.01f;
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

