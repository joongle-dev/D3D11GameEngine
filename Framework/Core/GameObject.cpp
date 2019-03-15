#include "Framework.h"
#include "GameObject.h"

GameObject::GameObject(Context* context) :
	m_context(context),
	m_name("Empty Object")
{
	//Resize component vector to match number of component classes
	m_components.resize(Util::FamilyTypeID<IComponent>::GetCount(), nullptr);

	//Create transform component
	AddComponent<Transform>();
}

GameObject::~GameObject()
{
	//Destroy all components
	for (IComponent* component : m_components)
		if (component) delete component;
}

void GameObject::Update()
{
	//Update all components
	for (IComponent* component : m_components)
		if (component) component->Update();
}