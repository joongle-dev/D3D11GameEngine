#pragma once

class Scene
{
public:
	Scene(class Context* context);
	~Scene();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void Update();

	class GameObject* Instantiate();
	class GameObject* Instantiate(class GameObject* original);
	class GameObject* Instantiate(class GameObject* original, class Transform* parent);
	void Destroy(GameObject* gameobject);

private:
	void Update(class Transform* transform);

private:
	class Context* m_context;

	//GameObjects in the scene
	Util::HandleTable<GameObject, 64, 64> m_objects;
	class Transform* m_root;
};