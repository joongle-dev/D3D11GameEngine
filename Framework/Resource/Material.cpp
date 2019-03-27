#include "Framework.h"
#include "Material.h"

Material::Material(Context * context) :
	Resource<Material>(context)
{
	m_renderer = context->GetSubsystem<Renderer>();
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

	unsigned short flags = TEX_ALBEDO | TEX_ROUGHNESS | TEX_NORMAL;
	m_shader = m_renderer->GetMatchingShader(flags);
}
