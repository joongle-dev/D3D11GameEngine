#include "Framework.h"
#include "GameObject.h"

GameObject::GameObject(Context* context, Scene* scene) :
	m_context(context),
	m_scene(scene),
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
		m_scene->DestroyComponent(component);
}

void GameObject::Update()
{
	//Update all components
	for (IComponent* component : m_components)
		if (component) component->Update();
}

void GameObject::Destroy()
{
	m_scene->Destroy(this);
}
