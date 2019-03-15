#include "Framework.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(Context * context) :
	Subsystem<ResourceManager>(context)
{
	m_resources.resize(Util::FamilyTypeID<IResource>::GetCount());
	m_filenames.resize(Util::FamilyTypeID<IResource>::GetCount());
	m_names.resize(Util::FamilyTypeID<IResource>::GetCount());
}

ResourceManager::~ResourceManager()
{
}
