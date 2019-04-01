#pragma once

class Geometry
{
public:
	static void CreateCube(Geometry& geometry, float radius = 0.5f);
	static void CreateSphere(Geometry& geometry, float radius = 0.5f, unsigned int lattitude = 20, unsigned int longitude = 40);

public:
	void AddVertex(Vector3 position, Vector3 normal, Vector2 uv);
	void AddVertex(Vector3 position, Vector3 normal, Vector3 tangent, Vector2 uv);
	void AddIndex(uint32_t index);
	void GenerateTangents();
	void Clear();
	void Optimize();

public:
	std::vector<uint32_t> indices;
	std::vector<Vector3>  positions;
	std::vector<Vector3>  normals;
	std::vector<Vector3>  tangents;
	std::vector<Vector3>  binormals;
	std::vector<Vector2>  uvs;
};