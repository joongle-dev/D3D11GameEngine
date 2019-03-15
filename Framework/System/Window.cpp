#include "Framework.h"
#include "Window.h"


Window::Window(Context* context, const std::wstring& name, const unsigned int& width, const unsigned int& height, const DWORD& style) :
	Subsystem<Window>(context), 
	m_name(name)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, style, FALSE);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = name.c_str();
	wcex.lpszClassName = name.c_str();
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	m_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		name.c_str(), name.c_str(),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr, nullptr,
		hInstance, this);
	assert(m_handle, "Window initialization failed");

	Show();
}

Window::~Window()
{
	DestroyWindow(m_handle);
	UnregisterClass(m_name.c_str(), m_instance);
}

void Window::Update()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) 
		m_context->SetActive(false);

	for (auto& func : m_subscribers)
		func(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

void Window::Show()
{
	SetForegroundWindow(m_handle);
	SetFocus(m_handle);
	ShowWindow(m_handle, SW_SHOWNORMAL);
	UpdateWindow(m_handle);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_QUIT: case WM_CLOSE:
		{
			PostQuitMessage(0);
		}	return 0;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}