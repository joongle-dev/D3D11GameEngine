#pragma once

class Context;
class GameObject;
class IComponent;
class Transform;

class Scene
{
public:
	Scene(class Context* context);
	~Scene();

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
		if (m_components[T::ComponentID] == nullptr)
			m_components[T::ComponentID] = new ComponentContainer<T>;

		return static_cast<ComponentContainer<T>*>(m_components[T::ComponentID]);
	}

	std::vector<IComponentContainer*> m_components;

public:
	//Component iterator
	template <class T>
	using ComponentIterator = typename ComponentContainer<T>::iterator;

	template <class T>
	ComponentIterator<T> ComponentBegin() { return GetComponentContainer<T>()->begin(); }

	template <class T>
	ComponentIterator<T> ComponentEnd() { return GetComponentContainer<T>()->end(); }

public:
	template <class T>
	T* CreateComponent(GameObject* owner)
	{
		//Return newly allocated and constructed component
		return new(GetComponentContainer<T>()->Allocate()) T(m_context, owner);
	}

	template <class T>
	void DestroyComponent(T* component)
	{
		if (!component) return;

		//Call destructor and deallocate component
		delete component;
		GetComponentContainer<T>()->Deallocate(component);
	}
	//Destroy component of unknown type
	void DestroyComponent(IComponent* component);
};