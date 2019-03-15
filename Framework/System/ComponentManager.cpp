#include "Framework.h"
#include "ComponentManager.h"

ComponentManager::ComponentManager(Context * context) :
	Subsystem<ComponentManager>(context)
{
	//Resize component container vector to match the number of component classes
	m_containers.resize(Util::FamilyTypeID<IComponent>::GetCount(), nullptr);
}

ComponentManager::~ComponentManager()
{
	//Destroy all component containers
	for (auto container : m_containers)
		if (container) delete container;
}
