#pragma once
#include "Framework.h"

class Geometry
{
public:
	static void CreateCube(Geometry& geometry, float length = 1.0f)
	{
		length *= 0.5f;

		geometry.Clear();

		//front
		geometry.AddVertex({ -length, -length, -length }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ -length, +length, -length }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 0.0f });
		geometry.AddVertex({ +length, -length, -length }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 1.0f });
		geometry.AddVertex({ +length, +length, -length }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 0.0f });
				
		//bottom
		geometry.AddVertex({ -length, -length, +length }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ -length, -length, -length }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 0.0f });
		geometry.AddVertex({ +length, -length, +length }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 1.0f });
		geometry.AddVertex({ +length, -length, -length }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 0.0f });
					
		//back		
		geometry.AddVertex({ -length, -length, +length }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f });
		geometry.AddVertex({ -length, +length, +length }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 0.0f });
		geometry.AddVertex({ +length, -length, +length }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ +length, +length, +length }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f });
					
		//top		
		geometry.AddVertex({ -length, +length, +length }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 0.0f });
		geometry.AddVertex({ -length, +length, -length }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ +length, +length, +length }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 0.0f });
		geometry.AddVertex({ +length, +length, -length }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 1.0f });
						
		//left			
		geometry.AddVertex({ -length, -length, +length }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ -length, +length, +length }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f });
		geometry.AddVertex({ -length, -length, -length }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f });
		geometry.AddVertex({ -length, +length, -length }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f });
					
		//right		
		geometry.AddVertex({ +length, -length, +length }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f });
		geometry.AddVertex({ +length, +length, +length }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f });
		geometry.AddVertex({ +length, -length, -length }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f });
		geometry.AddVertex({ +length, +length, -length }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f });

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
	static void CreateSphere(Geometry& geometry, float radius = 0.5f, unsigned int lattitude = 10, unsigned int longitude = 20)
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
				geometry.AddIndex((longitude + 1) * y + x);
				geometry.AddIndex((longitude + 1) * (y + 1) + x);
				geometry.AddIndex((longitude + 1) * y + x + 1);
				geometry.AddIndex((longitude + 1) * y + x + 1);
				geometry.AddIndex((longitude + 1) * (y + 1) + x);
				geometry.AddIndex((longitude + 1) * (y + 1) + x + 1);
			}
		geometry.Optimize();
	}

public:
	void AddVertex(Vector3 position, Vector3 normal, Vector2 uv)
	{
		positions.emplace_back(position);
		normals.emplace_back(normal);
		uvs.emplace_back(uv);
	}
	void AddIndex(uint32_t index)
	{
		indices.emplace_back(index);
	}
	void Clear()
	{
		indices.clear();
		positions.clear();
		normals.clear();
		uvs.clear();
	}
	void Optimize()
	{
		indices.shrink_to_fit();
		positions.shrink_to_fit();
		normals.shrink_to_fit();
		uvs.shrink_to_fit();
	}

public:
	std::vector<uint32_t> indices;
	std::vector<Vector3>  positions;
	std::vector<Vector3>  normals;
	std::vector<Vector3>  tangents;
	std::vector<Vector3>  binormals;
	std::vector<Vector2>  uvs;
};