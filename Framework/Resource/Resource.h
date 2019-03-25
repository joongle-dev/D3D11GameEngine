#pragma once
#include "Framework.h"

class IResource
{
public:
	IResource(class Context* context)
	{
		m_manager = context->GetSubsystem<class ResourceManager>();
	}
	virtual ~IResource() = default;

	virtual void LoadFromFile(std::string& path) = 0;

	const std::string& GetName() const { return m_name; }

protected:
	class ResourceManager* m_manager;

	std::string m_name;
	std::string m_filepath;
};

template <class T>
class Resource : public IResource
{
public:
	static inline const Util::TypeID ResourceID = Util::FamilyTypeID<IResource>::GetID<T>();

	virtual void LoadFromFile(std::string& path) {};

protected:
	Resource(class Context* context) : IResource(context) {}
	virtual ~Resource() = default;
};