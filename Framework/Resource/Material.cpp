#include "Framework.h"
#include "Material.h"

Material::Material(Context * context) :
	Resource<Material>(context),
	mBaseColor(1, 1, 1, 1),
	mUVOffset(0, 0),
	mUVTiling(1, 1)
{
}

void Material::LoadFromFile(const std::string & path)
{
	FileStreamRead file;
	file.Open(path);

	std::string stringbuf;

	file.Read(stringbuf);
	m_name = stringbuf;

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[TEX_ALBEDO] = m_manager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[TEX_ROUGHNESS] = m_manager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		mTextures[TEX_NORMAL] = m_manager->Load<Texture>(stringbuf);
}

Material::MaterialType Material::GetMaterialFlags() const
{
	unsigned char flags = 0;

	flags |= mTextures.count(TEX_ALBEDO) ? TEX_ALBEDO : 0;
	flags |= mTextures.count(TEX_ROUGHNESS) ? TEX_ROUGHNESS : 0;
	flags |= mTextures.count(TEX_METALLIC) ? TEX_METALLIC : 0;
	flags |= mTextures.count(TEX_NORMAL) ? TEX_NORMAL : 0;
	flags |= mTextures.count(TEX_HEIGHT) ? TEX_HEIGHT : 0;

	return static_cast<MaterialType>(flags);
}
