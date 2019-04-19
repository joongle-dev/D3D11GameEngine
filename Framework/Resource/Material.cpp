#include "Framework.h"
#include "Material.h"

Material::Material(Context * context) :
	Resource<Material>(context),
	mBaseColor(1, 1, 1, 1),
	mUVOffset(0, 0),
	mUVTiling(1, 1)
{
	ZeroMemory(mTextures, sizeof(Texture*) * Unknown);
}

void Material::LoadFromFile(const std::string & path)
{
	FileStreamRead file(path);

	std::string stringbuf;

	file.Read(stringbuf);
	mName = stringbuf;

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[Albedo] = mResourceManager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[Roughness] = mResourceManager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[Normal] = mResourceManager->Load<Texture>(stringbuf);
}

void Material::SaveToFile(const std::string & path)
{
}

unsigned int Material::GetShaderFlags() const
{
	unsigned int flags = 0;

	flags |= mTextures[Albedo]    ? ALBEDO_TEXTURE    : 0;
	flags |= mTextures[Roughness] ? ROUGHNESS_TEXTURE : 0;
	flags |= mTextures[Metallic]  ? METALLIC_TEXTURE  : 0;
	flags |= mTextures[Emissive]  ? EMISSIVE_TEXTURE  : 0;
	flags |= mTextures[Normal]    ? NORMAL_TEXTURE    : 0;
	flags |= mTextures[Height]    ? HEIGHT_TEXTURE    : 0;

	return flags;
}