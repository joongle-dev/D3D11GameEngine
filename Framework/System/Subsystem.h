#pragma once

class ISubsystem
{
public:
	virtual ~ISubsystem() = default;

	virtual void Update() = 0;

protected:
	ISubsystem() = default;

	virtual LRESULT MsgProc(HWND, const UINT&, const WPARAM&, const LPARAM&) = 0;
};

template <class T>
class Subsystem : public ISubsystem
{
public:
	static inline const Util::TypeID SubsystemID = Util::FamilyTypeID<ISubsystem>::GetID<T>();

	virtual void Update() {}

protected:
	Subsystem(class Context* context) : m_context(context) {}
	virtual ~Subsystem() = default;

	virtual LRESULT MsgProc(HWND, const UINT&, const WPARAM&, const LPARAM&) { return TRUE; };

	class Context* m_context;
};