#pragma once
#include "Subsystem.h"

class Window final : public Subsystem<Window>
{
public:
	Window(class Context* context, const std::wstring& name, const unsigned int& width, const unsigned int& height, const DWORD& style = WS_OVERLAPPEDWINDOW);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void Update();

	void Show();

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

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};