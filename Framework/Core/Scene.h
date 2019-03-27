#pragma once

class Context;
class GameObject;
class IComponent;
class Transform;

class Scene
{
public:
	Scene(class Context* context);
	~Scene() = default;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void Update();

	//Accessors
	const std::string& GetName() const { return m_name; }
	Transform* GetRoot() const { return m_root.get(); }

private:
	void Update(Transform* transform);

private:
	class Context* m_context;

	std::string m_name;
	std::unique_ptr<Transform> m_root;

//===================================================================================
//	  GameObjects
//===================================================================================
private:
	//GameObject container and handle table
	using GameObjectContainer = ChunkAllocator<GameObject, 128>;

	std::unique_ptr<GameObjectContainer> m_objects;

	Util::HandleTable<GameObject, 64, 64> m_handles;

public:
	//GameObject iterator
	using GameObjectIterator = GameObjectContainer::iterator;

	GameObjectIterator SceneBegin() { return m_objects->begin(); }

	GameObjectIterator SceneEnd() { return m_objects->end(); }

public:
	//GameObject instantiators and destroyers
	GameObject* Instantiate();

	GameObject* Instantiate(GameObject* original, Transform* parent = nullptr);

	void Destroy(GameObject* gameobject);

//=====================================================================================
//    Components
//=====================================================================================
private:
	//Component container
	using IComponentContainer = IChunkAllocator;

	template <class T>
	using ComponentContainer = ChunkAllocator<T, 128>;

	template <class T>
	ComponentContainer<T>* GetComponentContainer()
	{
		//If the component container does not exist, create one
		if (!m_components[T::ComponentID])
			m_components[T::ComponentID] = std::make_unique<ComponentContainer<T>>();

		return static_cast<ComponentContainer<T>*>(m_components[T::ComponentID].get());
	}

	std::vector<std::unique_ptr<IComponentContainer>> m_components;

public:
	//Component iterator
	template <class T>
	using ComponentIterator = typename ComponentContainer<T>::iterator;

	template <class T>
	ComponentIterator<T> ComponentBegin() { return GetComponentContainer<T>()->begin(); }

	template <class T>
	ComponentIterator<T> ComponentEnd() { return GetComponentContainer<T>()->end(); }

public:
	//Component instantiators and destroyers
	template <class T>
	T* CreateComponent(GameObject* owner)
	{
		//Return newly allocated component
		return GetComponentContainer<T>()->allocate(m_context, owner);
	}

	void DestroyComponent(IComponent* component);
};