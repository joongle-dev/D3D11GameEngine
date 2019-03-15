#include "Framework.h"
#include "Scene.h"

Scene::Scene(Context* context) :
	m_context(context), m_name("Test Scene")
{
	m_root = new Transform(context, nullptr);

	RenderTarget* temp = new RenderTarget(context);
	temp->Create(800, 400);

	GameObject* object = Instantiate();
	object->AddComponent<Camera>()->SetRenderTarget(temp);
	object->SetName("Main Camera");
}

Scene::~Scene() {}

void Scene::Update()
{
	for (int i = 0; i < m_root->GetChildCount(); i++)
		Update(m_root->GetChild(i));
}

GameObject * Scene::Instantiate()
{
	//Allocate new GameObject
	Util::Handle handle = m_objects.AllocateHandle(new GameObject(m_context));
	GameObject* object = m_objects[handle];
	object->m_instanceid = handle.index;

	object->GetComponent<Transform>()->SetParent(m_root);

	return object;
}

GameObject * Scene::Instantiate(GameObject * original)
{
	return Instantiate(original, m_root);
}

GameObject * Scene::Instantiate(GameObject * original, Transform * parent)
{
	//Allocate new GameObject
	Util::Handle handle = m_objects.AllocateHandle(new GameObject(m_context));
	GameObject* object = m_objects[handle];
	object->m_instanceid = handle.index;

	//Clone all the components of original GameObject
	for (int i = 1; i < original->m_components.size(); i++)
	{
		//TODO:
	}
	
	//Iterate original's children and instantiate them
	Transform* transform = object->GetComponent<Transform>();
	transform->SetParent(parent);
	for (int i = 0; i < transform->GetChildCount(); i++)
		transform->AddChild(Instantiate(transform->GetChild(i)->owner, transform)->GetComponent<Transform>());

	return object;
}

void Scene::Destroy(GameObject * gameObject)
{
	Util::Handle handle = m_objects[gameObject->m_instanceid];
	m_objects.ReleaseHandle(handle);
}

void Scene::Update(Transform * transform)
{
	transform->owner->Update();
	for (int i = 0; i < transform->GetChildCount(); i++)
		Update(transform->GetChild(i));
}