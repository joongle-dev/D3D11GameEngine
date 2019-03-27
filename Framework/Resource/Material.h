#pragma once
#include "Resource.h"

enum TextureType : unsigned short
{
	TEX_ALBEDO = 1 << 0,
	TEX_ROUGHNESS = 1 << 1,
	TEX_METALLIC = 1 << 2,
	TEX_EMISSIVE = 1 << 3,
	TEX_NORMAL = 1 << 4,
	TEX_DISPLACEMENT = 1 << 5,
};

class Material final : public Resource<Material>
{
public:
	Material(class Context* context);
	~Material() = default;

	void LoadFromFile(const std::string& path) override;

	class Shader* GetShader() const { return m_shader; }
	class Texture* GetTexture(const TextureType type) { return m_textures[type]; }

private:
	class Renderer* m_renderer;
	class Shader* m_shader;

	Vector3 m_baseColor;
	Vector2 m_uvTiling;
	Vector2 m_uvOffset;

	std::map<TextureType, class Texture*> m_textures;
};