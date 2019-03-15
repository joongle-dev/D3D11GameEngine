#pragma once
#include "Subsystem.h"

class Context;
class GameObject;
class IComponent;

class ComponentManager final : public Subsystem<ComponentManager>
{
private:
	//Component container============================================================
	using IComponentContainer = IChunkAllocator;
	template <class T>

	using ComponentContainer = ChunkAllocator<T, 128>;
	template <class T>

	ComponentContainer<T>* GetComponentContainer()
	{
		//If the component container does not exist, create one
		if (m_containers[T::ComponentID] == nullptr)
			m_containers[T::ComponentID] = new ComponentContainer<T>;

		return static_cast<ComponentContainer<T>*>(m_containers[T::ComponentID]);
	}
	
public:
	//Component iterator=============================================================
	template <class T>
	using ComponentIterator = typename ComponentContainer<T>::iterator;

	template <class T>
	ComponentIterator<T> begin() { return GetComponentContainer<T>()->begin(); }

	template <class T>
	ComponentIterator<T> end() { return GetComponentContainer<T>()->end(); }

public:
	//Component manager==============================================================
	ComponentManager(Context* context);
	~ComponentManager();

	template <class T>
	T* CreateComponent(GameObject* owner)
	{
		//Return newly allocated and constructed component
		return new(GetComponentContainer<T>()->Allocate()) T(m_context, owner);
	}

	template <class T>
	void RemoveComponent(IComponent* component)
	{
		//Call destructor and deallocate component
		component->~IComponent();
		GetComponentContainer<T>()->Deallocate(static_cast<T*>(component));
	}

private:
	std::vector<IComponentContainer*> m_containers;
};//=================================================================================