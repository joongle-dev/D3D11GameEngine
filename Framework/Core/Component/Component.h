#pragma once

class IComponent
{
	friend class GameObject;
public:
	virtual ~IComponent() = default;

	virtual void Update() = 0;

	__declspec(property(get = GetOwner)) class GameObject* owner;
	class GameObject* GetOwner() const { return m_owner; }

protected:
	IComponent(class Context* context, class GameObject* owner) : m_context(context), m_owner(owner) {};

	class GameObject* m_owner;
	class Context* m_context;
};

template <class T>
class Component : public IComponent
{
public:
	static inline const Util::TypeID ComponentID = Util::FamilyTypeID<IComponent>::GetID<T>();

	virtual void Update() {}

protected:
	Component(class Context* context = nullptr, class GameObject* owner = nullptr) : IComponent(context, owner) {}
	virtual ~Component() = default;
};