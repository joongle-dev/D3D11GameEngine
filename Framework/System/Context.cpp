#include "Framework.h"
#include "Context.h"

Context::Context() :
	m_active(true)
{
	//Resize subsystem vector to match the number of subsystem classes
	m_subsystems.resize(Util::FamilyTypeID<ISubsystem>::GetCount(), nullptr);
}

Context::~Context()
{
	//Destroy all subsystems
	for (auto& subsystem : m_subsystems)
		if (subsystem) delete subsystem;
}

void Context::Update()
{
	//Update all subsystems
	for (auto subsystem : m_subsystems)
		if (subsystem) subsystem->Update();
}
