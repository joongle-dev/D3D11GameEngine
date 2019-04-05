#pragma once
#include "Resource.h"

class Material final : public Resource<Material>
{
public:
	enum MaterialType : unsigned char
	{
		TEX_ALBEDO    = 1 << 0,
		TEX_ROUGHNESS = 1 << 1,
		TEX_METALLIC  = 1 << 2,
		TEX_EMISSIVE  = 1 << 3,
		TEX_NORMAL    = 1 << 4,
		TEX_HEIGHT    = 1 << 5,
	};

	using TextureMap = std::map<MaterialType, class Texture*>;

public:
	Material(class Context* context);
	~Material() = default;

	void LoadFromFile(const std::string& path) override;

	class Texture* GetTexture(const MaterialType type) { return mTextures[type]; }
	void SetTexture(const MaterialType type, class Texture* texture) { mTextures[type] = texture; }

	MaterialType GetMaterialFlags() const;

public:
	void UpdateShader();

private:
	Color   mBaseColor;
	Vector2 mUVOffset;
	Vector2 mUVTiling;

	TextureMap mTextures;
};