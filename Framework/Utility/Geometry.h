#pragma once
#include "Framework.h"

class Geometry
{
public:
	void CreateSphere(float radius, unsigned int lattitude = 10, unsigned int longitude = 20)
	{
		float phiStep = static_cast<float>(DirectX::XM_PI / lattitude);
		float thetaStep = static_cast<float>(DirectX::XM_2PI / longitude);

		for (int i = 1; i <= lattitude - 1; i++) {
			float phi = i * phiStep;
			for (int j = 0; j <= longitude; j++) {
				float theta = j * thetaStep;
				positions.emplace_back(
					radius * sin(phi) * cos(theta),
					radius * cos(phi),
					radius * sin(phi) * sin(theta));
				uvs.emplace_back(
					static_cast<float>(theta / DirectX::XM_2PI),
					static_cast<float>(phi / DirectX::XM_PI));
			}
		}
		positions.emplace_back(0, -radius, 0);
		uvs.emplace_back(0, 1);

		for (int i = 1; i <= longitude; i++) {
			indices.emplace_back(0);
			indices.emplace_back(i + 1);
			indices.emplace_back(i);
		}

		int baseIndex = 1;
		int ringVertexCount = longitude + 1;

		for (int i = 0; i < lattitude - 2; i++)
			for (int j = 0; j < longitude; j++)
			{
				indices.emplace_back(baseIndex + i * ringVertexCount + j);
				indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
				indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

				indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
				indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
				indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}

		int southPoleIndex = static_cast<int>(positions.size() - 1);
		baseIndex = southPoleIndex - ringVertexCount;

		for (int i = 0; i < longitude; i++)
		{
			indices.emplace_back(southPoleIndex);
			indices.emplace_back(baseIndex + i);
			indices.emplace_back(baseIndex + i + 1);
		}
	}

public:
	std::vector<uint32_t> indices;
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> uvs;
};