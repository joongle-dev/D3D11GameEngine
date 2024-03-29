#pragma once

class GameObject final
{
	friend class Scene;
public:
	GameObject(class Context* context, class Scene* scene);
	~GameObject();
	
	GameObject(const GameObject& rhs) = delete;
	GameObject& operator=(const GameObject& rhs) = delete;

	void Update();

	template <class T>
	T* AddComponent();
	template <class T>
	T* GetComponent() const;
	template <class T>
	void RemoveComponent();

	void Destroy();

	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }
	
	const size_t& GetInstanceID() const { return m_instanceid; }

	class IComponent* operator[](size_t index) { return m_components[index]; }

private:
	class Context* m_context;
	class Scene* m_scene;

	size_t m_instanceid;
	std::string	m_name;
	std::vector<class IComponent*> m_components;
};


template<class T>
inline T* GameObject::AddComponent()
{
	assert((m_components[T::ComponentID] == nullptr, "Component already exists"));

	T* temp = m_scene->CreateComponent<T>(this);
	m_components[T::ComponentID] = temp;
	return temp;
}

template<class T>
inline T * GameObject::GetComponent() const
{
	return static_cast<T*>(m_components[T::ComponentID]);
}

template<class T>
inline void GameObject::RemoveComponent()
{
	if (m_components[T::ComponentID]) {
		m_scene->DestroyComponent(m_components[T::ComponentID]);
		m_components[T::ComponentID] = nullptr;
	}
}
