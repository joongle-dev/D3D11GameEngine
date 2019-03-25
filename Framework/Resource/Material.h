#pragma once
#include "Resource.h"

enum class TextureType : unsigned char
{
	Albedo,
	Roughness,
	Metallic,
	Emissive,
	Ambient,
	Normal,
	Displacement,
};

class Material final : public Resource<Material>
{
public:
	Material(class Context* context);
	~Material() = default;

	void LoadFromFile(std::string& path) override;

private:
	Vector3 m_baseColor;
	Vector2 m_uvTiling;
	Vector2 m_uvOffset;

	std::map<TextureType, class Texture*> m_textures;
};