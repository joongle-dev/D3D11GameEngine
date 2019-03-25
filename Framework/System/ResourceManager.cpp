#include "Framework.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(Context * context) :
	Subsystem<ResourceManager>(context)
{
	m_directories.resize(Util::FamilyTypeID<IResource>::GetCount());
	m_resources.resize(Util::FamilyTypeID<IResource>::GetCount());
	m_filenames.resize(Util::FamilyTypeID<IResource>::GetCount());
	m_names.resize(Util::FamilyTypeID<IResource>::GetCount());

	RegisterDirectory<Texture>("../Assets/Texture/");
	RegisterDirectory<Mesh>("../Assets/Mesh/");
	RegisterDirectory<Material>("../Assets/Material/");
}

ResourceManager::~ResourceManager()
{
}
