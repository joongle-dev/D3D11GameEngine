#pragma once
#include "Resource.h"

class Material final : public Resource<Material>
{
public:
	enum TextureType : unsigned int
	{
		Albedo,
		Roughness,
		Metallic,
		Emissive,
		Normal,
		Height,
		NUM_TEX_TYPE,
	};

public:
	Material(class Context* context);
	~Material() = default;

	void LoadFromFile(const std::string& path) override;

	ID3D11ShaderResourceView* GetShaderResource(const TextureType type) { return mTextures[type] ? mTextures[type]->GetShaderResource() : nullptr; }
	class Texture* GetTexture(const TextureType type) { return mTextures[type]; }
	void SetTexture(const TextureType type, class Texture* texture) { mTextures[type] = texture; }

	unsigned int GetShaderFlags() const;

private:
	Color   mBaseColor;
	Vector2 mUVOffset;
	Vector2 mUVTiling;

	class Texture* mTextures[NUM_TEX_TYPE];
};