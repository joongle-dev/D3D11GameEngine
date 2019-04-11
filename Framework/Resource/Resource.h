#pragma once
#include "Framework.h"

class IResource
{
public:
	IResource(class Context* context)
	{
		mResourceManager = context->GetSubsystem<class ResourceManager>();
	}
	virtual ~IResource() = default;

	virtual void LoadFromFile(const std::string& path) = 0;

	const std::string& GetName() const { return mName; }

protected:
	class Context* mContext;
	class ResourceManager* mResourceManager;

	std::string mName;
	std::string mPath;
};

template <class T>
class Resource : public IResource
{
public:
	static inline const Util::TypeID ResourceID = Util::FamilyTypeID<IResource>::GetID<T>();

	virtual void LoadFromFile(const std::string& path) {};

protected:
	Resource(class Context* context) : IResource(context) {}
	virtual ~Resource() = default;
};