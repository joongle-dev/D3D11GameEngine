#pragma once
#include "Subsystem.h"

class ResourceManager final : public Subsystem<ResourceManager>
{
private:
	using ResourceVec = std::vector<class IResource*>;
	using ResourceMap = std::map<std::string, IResource*>;

	class IResourceContainer {};

	template <class T>
	class ResourceContainer : public IResourceContainer
	{
	public:
		T* AddResource(class IResource* resource)
		{
			return mResources.emplace_back(static_cast<T*>(resource));
		}
		std::vector<T*>& GetResources()
		{
			return mResources;
		}
	private:
		std::vector<T*> mResources;
	};

	template <class T>
	ResourceContainer<T>* GetResourceContainer()
	{
		if (!m_resources[T::ResourceID])
			m_resources[T::ResourceID] = std::make_unique<ResourceContainer<T>>();

		return static_cast<ResourceContainer<T>*>(m_resources[T::ResourceID].get());
	}
	std::vector<std::unique_ptr<IResourceContainer>> m_resources;

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

	template <class T>
	std::vector<T*>& GetResources();

	template <class T>
	void RegisterDirectory(std::string path);

	template <class T>
	const std::string& GetDirectory();

private:
	std::vector<std::string> m_directories;
	std::vector<ResourceMap> m_filenames;
	std::vector<ResourceMap> m_names;
};

template<class T>
inline T * ResourceManager::Load(std::string path)
{
	auto iter = m_filenames[T::ResourceID].emplace(path, new T(m_context));
	if (iter.second)
	{
		iter.first->second->LoadFromFile(path);
		GetResourceContainer<T>()->AddResource(iter.first->second);
		m_names[T::ResourceID][iter.first->second->GetName()] = iter.first->second;
	}
	return static_cast<T*>(iter.first->second);
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

template<class T>
inline std::vector<T*>& ResourceManager::GetResources()
{
	return GetResourceContainer<T>()->GetResources();
}

template<class T>
inline void ResourceManager::RegisterDirectory(std::string path)
{
	m_directories[T::ResourceID] = path;
}

template<class T>
inline const std::string & ResourceManager::GetDirectory()
{
	return m_directories[T::ResourceID];
}
