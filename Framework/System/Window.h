#pragma once
#include "Subsystem.h"

class Window final : public Subsystem<Window>
{
public:
	using ResizeProc  = std::function<void(const UINT&, const UINT&)>;
	using MessageProc = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

public:
	Window(class Context* context, const std::wstring& name, const unsigned int& width, const unsigned int& height, const DWORD& style = WS_OVERLAPPEDWINDOW);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void Update();

	void Show();

	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline void AttachResizeProc(ResizeProc proc) { 
		m_resize = proc; 
	}
	inline void AttachMessageProc(MessageProc proc) { 
		m_message = proc; 
	}
	inline const HWND& GetHandle() const { 
		return m_handle; 
	};
	inline const std::wstring& GetName() const { 
		return m_name; 
	}
	inline const UINT GetWidth() const{
		RECT rect;
		GetClientRect(m_handle, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}
	inline const UINT GetHeight() const{
		RECT rect;
		GetClientRect(m_handle, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}

private:
	HWND m_handle;
	HINSTANCE m_instance;
	std::wstring m_name;

	MessageProc m_message;
	ResizeProc m_resize;

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};