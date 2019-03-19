#pragma once

class IComponent
{
	friend class GameObject;
protected:
	class GameObject* m_owner;
	class Context* m_context;

public:
	IComponent(class Context* context, class GameObject* owner) : 
		m_context(context), m_owner(owner) {}
	virtual ~IComponent() = default;

	virtual void Update() = 0;
	virtual const Util::TypeID GetComponentID() const = 0;
	virtual const std::string GetComponentName() const = 0;
	virtual Json Serialize() = 0;
	virtual void Deserialize(Json& j) = 0;
	class GameObject* GetOwner() const { return m_owner; }
};

template <class Class>
class Component : public IComponent
{
protected:
	Component(class Context* context = nullptr, class GameObject* owner = nullptr) :
		IComponent(context, owner) {
		static bool temp = false;
		if (!temp) {
			
			static_cast<Class*>(this)->InitializeSerializable();
			temp = true;
		}
	}
	virtual ~Component() = default;

	virtual void InitializeSerializable()
	{
	
	}
	template <typename GET, typename SET>
	void RegisterSerializeField(std::string name, GET getter, SET setter)
	{
		Serializable<Class>::RegisterSerializeField(name, getter, setter);
	}
public:
	static inline const Util::TypeID ComponentID = Util::FamilyTypeID<IComponent>::GetID<Class>();

	virtual void Update() 
	{

	}
	virtual const Util::TypeID GetComponentID() const override 
	{ 
		return ComponentID; 
	}
	virtual const std::string GetComponentName() const override 
	{ 
		static const std::string name = std::string(typeid(Class).name()).substr(6); 
		return name; 
	}
	virtual Json Serialize() override
	{ 
		return Serializable<Class>::StaticSerialize(*static_cast<Class*>(this));
	}
	virtual void Deserialize(Json& j) override
	{ 
		Serializable<Class>::StaticDeserialize(*static_cast<Class*>(this), j); 
	}
};