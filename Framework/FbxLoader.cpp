#include "Framework.h"
#include "FbxLoader.h"

using namespace fbxsdk;
using namespace DirectX;

inline Vector3 FbxToVec3(FbxVector4 vec)
{
	return Vector3(
		(float)vec.mData[0],
		(float)vec.mData[1],
		(float)vec.mData[2]
	);
}
inline Vector4 FbxToVec4(FbxVector4 vec)
{
	return Vector4(
		(float)vec.mData[0],
		(float)vec.mData[1],
		(float)vec.mData[2],
		(float)vec.mData[3]
	);
}
inline Matrix FbxToMatrix(FbxAMatrix & mat)
{
	FbxVector4 r1 = mat.GetRow(0);
	FbxVector4 r2 = mat.GetRow(1);
	FbxVector4 r3 = mat.GetRow(2);
	FbxVector4 r4 = mat.GetRow(3);

	return Matrix
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);
}

XMFLOAT2 GetUV(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	XMFLOAT2 result = { 0.0f,0.0f };

	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == NULL) return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{
		case FbxLayerElement::eByControlPoint:
		{
			if (refMode == FbxLayerElement::eDirect)
			{
				result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
			}
			else if (refMode == FbxLayerElement::eIndexToDirect)
			{
				int index = uv->GetIndexArray().GetAt(cpIndex);
				result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
			}
		}
		break;
		case FbxLayerElement::eByPolygonVertex:
		{
			result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
		}
		break;
	}
	result.y = 1.0f - result.y;

	return result;
}

void FbxLoader::FbxSkinData::Insert(uint32_t index, float weight)
{
	FbxVertexBlends::iterator iter = blends.begin();
	while (iter != blends.end()) {
		if (weight >= iter->second) {
			blends.insert(iter, std::make_pair(index, weight));
			return;
		}
		iter++;
	}
	blends.push_back(std::make_pair(index, weight));
}

XMFLOAT4 FbxLoader::FbxSkinData::Indices()
{
	float indices[4] = { 0, 0, 0, 0 };
	for (int i = 0; i < blends.size() && i < 4; i++)
		indices[i] = blends[i].first;
	return XMFLOAT4(indices[0], indices[1], indices[2], indices[3]);
}

XMFLOAT4 FbxLoader::FbxSkinData::Weights()
{
	float weights[4] = { 0, 0, 0, 0 };
	float length = 0;
	for (int i = 0; i < blends.size() && i < 4; i++) {
		weights[i] = blends[i].second;
		length += weights[i] * weights[i];
	}
	length = sqrt(length);
	return XMFLOAT4(weights[0] / length, weights[1] / length, weights[2] / length, weights[3] / length);
}


void FbxLoader::FbxMeshData::SaveAsFile()
{
	FileStreamWrite file;
	file.Open(name + ".mesh");

	file.Write(name);
	file.Write(numVertices);
	file.Write(numIndices);
	file.Write(attribute);

	file.Write(indices.data(), numIndices * sizeof(uint32_t));
	file.Write(positions.data(), numVertices * sizeof(XMFLOAT3));
	file.Write(normals.data(), numVertices * sizeof(XMFLOAT3));
	file.Write(uvs.data(), numVertices * sizeof(XMFLOAT2));
	file.Write(blendIndices.data(), numVertices * sizeof(XMFLOAT4));
	file.Write(blendWeights.data(), numVertices * sizeof(XMFLOAT4));

	file.Close();
}

FbxLoader::FbxLoader()
{
	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);

	bool result;
	int major, minor, revision;
	FbxManager::GetFileFormatVersion(major, minor, revision);

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);
	importer = FbxImporter::Create(manager, "");
}

FbxLoader::~FbxLoader()
{
	importer->Destroy();
	ios->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void FbxLoader::Load(std::string filename)
{
	importer->Initialize(filename.c_str(), -1, ios);
	importer->Import(scene);
	converter = new FbxGeometryConverter(manager);

	ProcessMeshData(scene->GetRootNode());
}

void FbxLoader::ProcessBoneData(FbxNode * node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxBoneData bone;
			bone.index = index;
			bone.parentIndex = parent;
			bone.name = node->GetName();
			bones.push_back(bone);
		}
	}
	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessBoneData(node->GetChild(i), bones.size(), index);
}

void FbxLoader::ProcessMeshData(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eMesh)
		{
			converter->Triangulate(attribute, true, true);

			FbxMesh* fbxMesh = node->GetMesh();
			FbxMeshData fbxMeshData;

			//Get per control point skinning data
			std::vector<FbxSkinData> skinData = GetSkinData(fbxMesh);

			int polyCount = fbxMesh->GetPolygonCount();
			fbxMeshData.name = node->GetName();
			fbxMeshData.numVertices = 0;
			fbxMeshData.numIndices = polyCount * 3;
			fbxMeshData.attribute = VertexAttribute::NORMAL | VertexAttribute::UV | VertexAttribute::SKIN;
			fbxMeshData.indices.resize(fbxMeshData.numIndices);
			fbxMeshData.positions.reserve(polyCount);
			fbxMeshData.normals.reserve(polyCount);
			fbxMeshData.uvs.reserve(polyCount);
			fbxMeshData.blendIndices.reserve(polyCount);
			fbxMeshData.blendWeights.reserve(polyCount);

			using Vertex = std::tuple<Vector3, Vector3, Vector2>;
			auto vHash = [](const Vertex& v)->size_t 
			{
				size_t hash = 1;
				hash = hash * 31 + std::hash<float>()(std::get<0>(v).x);
				hash = hash * 31 + std::hash<float>()(std::get<0>(v).y);
				hash = hash * 31 + std::hash<float>()(std::get<0>(v).z);
				return hash;
			};
			auto vCompare = [](const Vertex& l, const Vertex& r)->bool 
			{
				return l == r;
			};
			std::unordered_map<Vertex, UINT, decltype(vHash), decltype(vCompare)> vLookUp(1000, vHash, vCompare);

			for (int i = 0; i < polyCount; i++)
			{
				for (int vi = 0; vi < 3; vi++)
				{
					//Control Point, UV, Vertex index
					int cpIndex = fbxMesh->GetPolygonVertex(i, vi);
					int uvIndex = fbxMesh->GetTextureUVIndex(i, vi);
					int vtIndex = fbxMesh->GetPolygonVertexIndex(i) + vi;

					if (cpIndex < 0 || cpIndex >= fbxMesh->GetControlPointsCount()) continue;

					FbxVector4 fbxPos = fbxMesh->GetControlPointAt(cpIndex);
					FbxVector4 fbxNormal;
					fbxMesh->GetPolygonVertexNormal(i, vi, fbxNormal);

					Vertex v = std::make_tuple(
						FbxToVec3(fbxPos), 
						FbxToVec3(fbxNormal), 
						GetUV(fbxMesh, cpIndex, uvIndex));

					auto iter = vLookUp.emplace(v, fbxMeshData.numVertices);
					if (iter.second) {
						fbxMeshData.indices[i * 3 + vi] = fbxMeshData.numVertices;
						fbxMeshData.positions.push_back(std::get<0>(v));
						fbxMeshData.normals.push_back(std::get<1>(v));
						fbxMeshData.uvs.push_back(std::get<2>(v));
						fbxMeshData.blendIndices.push_back(skinData[cpIndex].Indices());
						fbxMeshData.blendWeights.push_back(skinData[cpIndex].Weights());
						fbxMeshData.numVertices++;
					}
					else
						fbxMeshData.indices[i * 3 + vi] = iter.first->second;
				}
			}

			//Get material
			FbxSurfaceMaterial* material = node->GetMaterial(0);
			fbxMeshData.SaveAsFile();
		}
	}

	//Process child nodes
	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessMeshData(node->GetChild(i));
}

void FbxLoader::ProcessMaterials()
{
	for (size_t i = 0; i < scene->GetMaterialCount(); i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial material;
		material.name = fbxMaterial->GetName();


		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;
			material.diffuseColor.x = (float)lambert->Diffuse.Get().mData[0];
			material.diffuseColor.y = (float)lambert->Diffuse.Get().mData[1];
			material.diffuseColor.z = (float)lambert->Diffuse.Get().mData[2];
			material.diffuseColor.w = (float)lambert->DiffuseFactor;
		}

		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) == true)
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)fbxMaterial;
			material.specularColor.x = (float)phong->Specular.Get().mData[0];
			material.specularColor.y = (float)phong->Specular.Get().mData[1];
			material.specularColor.z = (float)phong->Specular.Get().mData[2];
			material.specularColor.w = (float)phong->SpecularFactor;
			material.specularPower = (float)phong->Shininess;
		}

		FbxProperty prop;

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid() == true)
		{
			if (prop.GetSrcObjectCount() > 0)
			{
				FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
				if (texture != NULL)
					material.diffuseTexture = std::string(texture->GetFileName());
				else
					material.diffuseTexture = "";
			}
		}

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		if (prop.IsValid() == true)
		{
			if (prop.GetSrcObjectCount() > 0)
			{
				FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
				if (texture != NULL)
					material.specularTexture = std::string(texture->GetFileName());
				else
					material.specularTexture = "";
			}
		}

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (prop.IsValid() == true)
		{
			if (prop.GetSrcObjectCount() > 0)
			{
				FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
				if (texture != NULL)
					material.normalTexture = std::string(texture->GetFileName());
				else
					material.normalTexture = "";
			}
		}

		materials.push_back(material);
	}
}

void FbxLoader::ProcessAnimationData()
{
}

void FbxLoader::ProcessKeyFrameData(FbxNode * node, int start, int end, int index)
{
}

std::vector<FbxLoader::FbxSkinData> FbxLoader::GetSkinData(FbxMesh * mesh)
{
	std::vector<FbxSkinData> skinData;
	skinData.assign(mesh->GetControlPointsCount(), FbxSkinData());

	for (int i = 0; i < mesh->GetDeformerCount(); i++)
	{
		FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(deformer);
		if (skin == NULL) continue;
		for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
			assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

			std::string linkName = cluster->GetLink()->GetName();
			int boneIndex = GetBoneIndex(linkName);

			FbxAMatrix transform;
			FbxAMatrix linkTransform;
			cluster->GetTransformMatrix(transform);
			cluster->GetTransformLinkMatrix(linkTransform);

			FbxAMatrix invBindpose = linkTransform.Inverse() * transform;
			//bones[boneIndex].global = FbxToMatrix(invBindpose);

			for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
			{
				int temp = cluster->GetControlPointIndices()[indexCount];
				double* weights = cluster->GetControlPointWeights();
				skinData[temp].Insert(boneIndex, (float)weights[indexCount]);
			}
		}
	}
	return skinData;
}

int FbxLoader::GetBoneIndex(std::string name)
{
	for (int i = 0; i < bones.size(); i++)
		if (bones[i].name == name) return i;
	return -1;
}

int FbxLoader::GetMaterialIndex(std::string name)
{
	for (size_t i = 0; i < materials.size(); i++)
		if (materials[i].name == name) return i;
	return -1;
}