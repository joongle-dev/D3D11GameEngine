#include "Framework.h"
#include "Geometry.h"

void Geometry::CreateCube(Geometry & geometry, float radius)
{
	geometry.Clear();

	//front
	geometry.AddVertex({ -radius, -radius, -radius }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ -radius, +radius, -radius }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 0.0f });
	geometry.AddVertex({ +radius, -radius, -radius }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 1.0f });
	geometry.AddVertex({ +radius, +radius, -radius }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 0.0f });

	//bottom
	geometry.AddVertex({ -radius, -radius, +radius }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ -radius, -radius, -radius }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 0.0f });
	geometry.AddVertex({ +radius, -radius, +radius }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 1.0f });
	geometry.AddVertex({ +radius, -radius, -radius }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 0.0f });

	//back		
	geometry.AddVertex({ -radius, -radius, +radius }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f });
	geometry.AddVertex({ -radius, +radius, +radius }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 0.0f });
	geometry.AddVertex({ +radius, -radius, +radius }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ +radius, +radius, +radius }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f });

	//top		
	geometry.AddVertex({ -radius, +radius, +radius }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 0.0f });
	geometry.AddVertex({ -radius, +radius, -radius }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ +radius, +radius, +radius }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 0.0f });
	geometry.AddVertex({ +radius, +radius, -radius }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 1.0f });

	//left			
	geometry.AddVertex({ -radius, -radius, +radius }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ -radius, +radius, +radius }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f });
	geometry.AddVertex({ -radius, -radius, -radius }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f });
	geometry.AddVertex({ -radius, +radius, -radius }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f });

	//right		
	geometry.AddVertex({ +radius, -radius, +radius }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f });
	geometry.AddVertex({ +radius, +radius, +radius }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f });
	geometry.AddVertex({ +radius, -radius, -radius }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f });
	geometry.AddVertex({ +radius, +radius, -radius }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f });

	//front
	geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
	geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);

	//bottom
	geometry.AddIndex(4); geometry.AddIndex(5); geometry.AddIndex(6);
	geometry.AddIndex(6); geometry.AddIndex(5); geometry.AddIndex(7);

	//back
	geometry.AddIndex(10); geometry.AddIndex(9); geometry.AddIndex(8);
	geometry.AddIndex(11); geometry.AddIndex(9); geometry.AddIndex(10);

	//top
	geometry.AddIndex(14); geometry.AddIndex(13); geometry.AddIndex(12);
	geometry.AddIndex(15); geometry.AddIndex(13); geometry.AddIndex(14);

	//left
	geometry.AddIndex(16); geometry.AddIndex(17); geometry.AddIndex(18);
	geometry.AddIndex(18); geometry.AddIndex(17); geometry.AddIndex(19);

	//right
	geometry.AddIndex(22); geometry.AddIndex(21); geometry.AddIndex(20);
	geometry.AddIndex(23); geometry.AddIndex(21); geometry.AddIndex(22);

	geometry.Optimize();
}

void Geometry::CreateSphere(Geometry & geometry, float radius, unsigned int lattitude, unsigned int longitude)
{
	geometry.Clear();

	for (float y = 0; y <= lattitude; y++)
		for (float x = 0; x <= longitude; x++) {
			DirectX::XMVECTOR vector = DirectX::XMVector3Rotate(
				DirectX::XMVectorSet(0, radius, 0, 1),
				DirectX::XMQuaternionRotationRollPitchYaw(
					DirectX::XM_PI * x / longitude,
					DirectX::XM_2PI * y / lattitude,
					0.0f));
			geometry.AddVertex(
				vector,
				DirectX::XMVector3Normalize(vector),
				Vector2(x / longitude, y / lattitude));
		}

	for (unsigned int y = 0; y < lattitude; y++)
		for (unsigned int x = 0; x < longitude; x++) {
			geometry.AddIndex((longitude + 1) * y + x + 1);
			geometry.AddIndex((longitude + 1) * (y + 1) + x);
			geometry.AddIndex((longitude + 1) * y + x);
			geometry.AddIndex((longitude + 1) * (y + 1) + x + 1);
			geometry.AddIndex((longitude + 1) * (y + 1) + x);
			geometry.AddIndex((longitude + 1) * y + x + 1);
		}
	geometry.Optimize();
}

void Geometry::AddVertex(Vector3 position, Vector3 normal, Vector2 uv)
{
	positions.emplace_back(position);
	normals.emplace_back(normal);
	uvs.emplace_back(uv);
}

void Geometry::AddVertex(Vector3 position, Vector3 normal, Vector3 tangent, Vector2 uv)
{
	AddVertex(position, normal, uv);
	tangents.emplace_back(tangent);
}

void Geometry::AddIndex(uint32_t index)
{
	indices.emplace_back(index);
}

void Geometry::GenerateTangents()
{
	tangents.resize(positions.size(), Vector3());
	binormals.resize(positions.size(), Vector3());

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		auto index0 = indices[i + 0];
		auto index1 = indices[i + 1];
		auto index2 = indices[i + 2];

		auto position0 = positions[index0];
		auto position1 = positions[index1];
		auto position2 = positions[index2];

		auto texcoord0 = uvs[index0];
		auto texcoord1 = uvs[index1];
		auto texcoord2 = uvs[index2];

		Vector3 e0 = position1 - position0;
		Vector3 e1 = position2 - position0;

		Vector2 u0 = texcoord1 - texcoord0;
		Vector2 u1 = texcoord2 - texcoord0;
		
		float r = 1.0f / (u0.x * u1.y - u1.x * u0.y);
		Vector3 tangent = (e0 * u1.y - e1 * u0.y) * r;
		Vector3 bitangent = (e0 * u1.x - e1 * u0.x) * r;

		tangents[index0] += tangent;
		tangents[index1] += tangent;
		tangents[index2] += tangent;
	}

	for (size_t i = 0; i < positions.size(); i++)
	{
		Vector3 n = normals[i];
		Vector3 t = tangents[i];

		tangents[i] = (t - n.Dot(t) * n);
		tangents[i].Normalize();
		binormals[i] = normals[i].Cross(tangents[i]);
	}
}

void Geometry::Clear()
{
	indices.clear();
	positions.clear();
	normals.clear();
	uvs.clear();
}

void Geometry::Optimize()
{
	indices.shrink_to_fit();
	positions.shrink_to_fit();
	normals.shrink_to_fit();
	uvs.shrink_to_fit();
}
