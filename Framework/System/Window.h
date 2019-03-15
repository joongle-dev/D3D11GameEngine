#pragma once
#include "Subsystem.h"

class Window final : public Subsystem<Window>
{
	using WndProcHandler = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

public:
	Window(class Context* context, const std::wstring& name, const unsigned int& width, const unsigned int& height, const DWORD& style = WS_OVERLAPPEDWINDOW);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void Update();

	void Show();

	inline void WndProcSubscribe(WndProcHandler func)
	{
		m_subscribers.push_back(func);
	}
	inline const HWND& GetHandle() const
	{ 
		return m_handle;
	};
	inline const std::wstring& GetName() const
	{
		return m_name;
	}
	inline const UINT GetWidth() const
	{
		RECT rect;
		GetClientRect(m_handle, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}
	inline const UINT GetHeight() const
	{
		RECT rect;
		GetClientRect(m_handle, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}

private:
	HWND m_handle;
	HINSTANCE m_instance;
	std::wstring m_name;

	std::vector<WndProcHandler> m_subscribers;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};