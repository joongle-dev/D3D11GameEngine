#include "Framework.h"
#include "Scene.h"

Scene::Scene(Context* context) :
	m_context(context), m_name("Test Scene")
{
	//Resize component container vector to match the number of component classes
	m_components.resize(Util::FamilyTypeID<IComponent>::GetCount());

	//Create GameObject container
	m_objects = std::make_unique<GameObjectContainer>();

	//Create root transform
	m_root = std::make_unique<Transform>(context, nullptr);

	//Create main camera object
	GameObject* object = Instantiate();
	object->AddComponent<Camera>();
	object->SetName("Main Camera");
}

void Scene::Update()
{
	//Update GameObjects in hierarchial order
	for (int i = 0; i < m_root->GetChildCount(); i++)
		Update(m_root->GetChild(i));
}

void Scene::Update(Transform * transform)
{
	transform->GetOwner()->Update();
	for (int i = 0; i < transform->GetChildCount(); i++)
		Update(transform->GetChild(i));
}

GameObject * Scene::Instantiate()
{
	//Allocate new GameObject and add to handle table
	GameObject* object = m_objects->Allocate(m_context, this);
	Util::Handle handle = m_handles.AllocateHandle(object);

	//Initialize GameObject
	object->m_instanceid = handle.index;
	auto parent = GetRoot();
	auto trans = object->GetComponent<Transform>();
	trans->SetParent(GetRoot());

	return object;
}

GameObject * Scene::Instantiate(GameObject * original, Transform * parent)
{
	if (!parent) 
		parent = GetRoot();

	GameObject* object = Instantiate();

	//TODO:

	return object;
}

void Scene::Destroy(GameObject * object)
{
	//Release handle to GameObject
	Util::Handle handle = m_handles[object->m_instanceid];
	m_handles.ReleaseHandle(handle);

	//Deallocate GameObject
	m_objects->Deallocate(object);
}

void Scene::DestroyComponent(IComponent * component)
{
	if (!component) return;

	//Deallocate component
	if (m_components[component->GetComponentID()])
		m_components[component->GetComponentID()]->Deallocate(component);
}