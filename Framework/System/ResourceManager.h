#pragma once
#include "Subsystem.h"

class ResourceManager final : public Subsystem<ResourceManager>
{
	typedef std::vector<class IResource*> ResourceVec;
	typedef std::map<std::string, IResource*> ResourceMap;
public:
	ResourceManager(class Context* context);
	~ResourceManager();

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	template <class T>
	T* Load(std::string path);

	template <class T>
	T* Get(std::string name);

	template <class T>
	T* GetByFilename(std::string path);

private:
	std::vector<ResourceVec> m_resources;
	std::vector<ResourceMap> m_filenames;
	std::vector<ResourceMap> m_names;
};

template<class T>
inline T * ResourceManager::Load(std::string path)
{
	T* temporary = new T(m_context);
	temporary->LoadFromFile(path);
	m_resources[T::ResourceID].push_back(temporary);
	m_names[T::ResourceID][temporary->GetName()] = temporary;
	return temporary;
}

template<class T>
inline T * ResourceManager::Get(std::string name)
{
	return static_cast<T*>(m_names[T::ResourceID][name]);
}

template<class T>
inline T * ResourceManager::GetByFilename(std::string path)
{
	return static_cast<T*>(m_filenames[T::ResourceID][path]);
}
