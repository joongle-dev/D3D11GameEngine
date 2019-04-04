#pragma once
#include "Resource.h"

class Material final : public Resource<Material>
{
public:
	enum MaterialType : unsigned char
	{
		TEX_ALBEDO = 1 << 0,
		TEX_ROUGHNESS = 1 << 1,
		TEX_METALLIC = 1 << 2,
		TEX_EMISSIVE = 1 << 3,
		TEX_NORMAL = 1 << 4,
		TEX_HEIGHT = 1 << 5,
	};

public:
	Material(class Context* context);
	~Material() = default;

	void LoadFromFile(const std::string& path) override;

	class Shader* GetShader() const { return m_shader; }
	class Texture* GetTexture(const MaterialType type) { return m_textures[type]; }
	void SetTexture(const MaterialType type, class Texture* texture) { m_textures[type] = texture; UpdateShader(); }

public:
	void UpdateShader();

private:
	class Renderer* m_renderer;
	class Shader* m_shader;

	Vector3 m_baseColor;
	Vector2 m_uvTiling;
	Vector2 m_uvOffset;

	std::map<MaterialType, class Texture*> m_textures;
};