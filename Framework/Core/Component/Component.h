#pragma once

class IComponent
{
	friend class GameObject;
public:
	IComponent(class Context* context, class GameObject* owner) : 
		m_context(context), m_owner(owner) {}
	virtual ~IComponent() = default;

	virtual void Update() = 0;

	virtual const Util::TypeID GetComponentID() const = 0;

	class GameObject* GetOwner() const { return m_owner; }

protected:
	class GameObject* m_owner;
	class Context* m_context;
};

template <class T>
class Component : public IComponent
{
protected:
	Component(class Context* context = nullptr, class GameObject* owner = nullptr) :
		IComponent(context, owner) {}
	virtual ~Component() = default;

public:
	static inline const Util::TypeID ComponentID = Util::FamilyTypeID<IComponent>::GetID<T>();

	virtual void Update() {}

	virtual const Util::TypeID GetComponentID() const override { return ComponentID; }
};