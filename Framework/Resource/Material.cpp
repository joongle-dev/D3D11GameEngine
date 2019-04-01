#include "Framework.h"
#include "Material.h"

Material::Material(Context * context) :
	Resource<Material>(context)
{
	m_renderer = context->GetSubsystem<Renderer>();

	m_shader = context->GetSubsystem<Renderer>()->GetMatchingShader(0);
	//UpdateShader();
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
		m_textures[TEX_ALBEDO] = m_manager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		m_textures[TEX_ROUGHNESS] = m_manager->Load<Texture>(stringbuf);

	file.Read(stringbuf);
	if (!stringbuf.empty())
		m_textures[TEX_NORMAL] = m_manager->Load<Texture>(stringbuf);

	UpdateShader();
}

void Material::UpdateShader()
{
	unsigned short flags = 0;
	
	flags |= m_textures.count(TEX_ALBEDO)    ? TEX_ALBEDO    : 0;
	flags |= m_textures.count(TEX_ROUGHNESS) ? TEX_ROUGHNESS : 0;
	flags |= m_textures.count(TEX_METALLIC)  ? TEX_METALLIC  : 0;
	flags |= m_textures.count(TEX_NORMAL)    ? TEX_NORMAL    : 0;
	flags |= m_textures.count(TEX_HEIGHT)    ? TEX_HEIGHT : 0;

	m_shader = m_renderer->GetMatchingShader(flags);
}
