#pragma once

class Context final
{
public:
	Context();
	~Context();

	Context(const Context&) = delete;
	Context& operator=(const Context&) = delete;

	template <class T, typename... Args>
	inline T* AddSubsystem(Args&&... arg);

	template <class T>
	inline T* GetSubsystem();

	void Update();

	void SetActive(bool active) { m_active = active; }
	const bool IsActive() const { return m_active; }

private:
	bool m_active;
	std::vector<class ISubsystem*> m_subsystems;
};

template <class T, typename... Args>
inline T* Context::AddSubsystem(Args&&... arg)
{
	assert(m_subsystems[T::SubsystemID] == nullptr, "Subsystem already exists.");

	m_subsystems[T::SubsystemID] = new T(this, std::forward<Args>(arg)...);
	return static_cast<T*>(m_subsystems[T::SubsystemID]);
}

template <class T>
inline T* Context::GetSubsystem()
{
	return static_cast<T*>(m_subsystems[T::SubsystemID]);
}