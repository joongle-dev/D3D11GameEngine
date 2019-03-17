#include "Framework.h"
#include "Scene.h"

Scene::Scene(Context* context) :
	m_context(context), m_name("Test Scene")
{
	//Resize component container vector to match the number of component classes
	m_components.resize(Util::FamilyTypeID<IComponent>::GetCount(), nullptr);

	//Create GameObject container
	m_objects = std::make_unique<GameObjectContainer>();

	//Create root transform
	m_root = std::make_unique<Transform>(context, nullptr);

	//Create main camera object
	GameObject* object = Instantiate();
	object->AddComponent<Camera>();
	object->SetName("Main Camera");
}

Scene::~Scene()
{
	//Destroy component containers
	for (auto components : m_components)
		if (components) delete components;
}

void Scene::Update()
{
	//Update GameObjects in hierarchial order
	for (int i = 0; i < m_root->GetChildCount(); i++)
		Update(m_root->GetChild(i));
}

GameObject * Scene::Instantiate()
{
	//Allocate new GameObject and add to handle table
	GameObject* object = new(m_objects->Allocate()) GameObject(m_context, this);
	Util::Handle handle = m_handles.AllocateHandle(object);

	//Initialize GameObject
	object->m_instanceid = handle.index;
	object->GetComponent<Transform>()->SetParent(GetRoot());

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

void Scene::Destroy(GameObject * gameObject)
{
	Util::Handle handle = m_handles[gameObject->m_instanceid];
	m_handles.ReleaseHandle(handle);
}

void Scene::DestroyComponent(IComponent * component)
{
	if (!component) return;

	//Call destructor and deallocate component
	delete component;
	if (m_components[component->GetComponentID()])
		m_components[component->GetComponentID()]->Deallocate(component);
}


void Scene::Update(Transform * transform)
{
	transform->GetOwner()->Update();
	for (int i = 0; i < transform->GetChildCount(); i++)
		Update(transform->GetChild(i));
}