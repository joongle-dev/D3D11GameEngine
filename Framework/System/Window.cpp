#include "Framework.h"
#include "Window.h"


Window::Window(Context* context, const std::wstring& name, const unsigned int& width, const unsigned int& height, const DWORD& style) :
	Subsystem<Window>(context), 
	m_name(name)
{
	m_instance = GetModuleHandle(NULL);

	//Adjustment window rect so that the given width and height are that of the client region size, not window
	RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&rect, style, FALSE);

	//Register window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);						 //Size of this structure
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		 //Window style
	wcex.lpfnWndProc = StaticWindowProc;					 //Window procedure
	wcex.cbClsExtra = 0;									 //
	wcex.cbWndExtra = sizeof(this);							 //Reserve space to store pointer to this instance
	wcex.hInstance = m_instance;							 //
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);		 //Icon
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);			 //Cursor
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); //Background color
	wcex.lpszMenuName = name.c_str();						 //Resource name
	wcex.lpszClassName = name.c_str();						 //Class name
	wcex.hIconSm = NULL;									 //
	RegisterClassEx(&wcex);

	//Create window
	m_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		name.c_str(), 
		name.c_str(),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr, 
		nullptr,
		m_instance,
		this);

	//Store pointer to this window class instance in handle
	SetWindowLongPtr(m_handle, 0, reinterpret_cast<LONG_PTR>(this));

	assert((m_handle, "Window initialization failed"));

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
}

void Window::Show()
{
	SetForegroundWindow(m_handle);
	SetFocus(m_handle);
	ShowWindow(m_handle, SW_SHOWNORMAL);
	UpdateWindow(m_handle);
}

LRESULT Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			//Call resize message handler if available
			if (m_resize)
				m_resize(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_QUIT: case WM_CLOSE:
		{
			m_context->SetActive(false);
			PostQuitMessage(0);
			break;
		}
		default:
		{
			//Call additional message handlers if available
			if (m_input)
				m_input(hWnd, msg, wParam, lParam);
			if (m_message)
				m_message(hWnd, msg, wParam, lParam);
			break;
		}
	}

	//Call default message handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::StaticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Receive instance pointer
	Window* instance = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, 0));

	//If instance is available, call its message handler
	if (instance)
		return instance->WindowProc(hWnd, msg, wParam, lParam);

	//Call default message handler otherwise
	return DefWindowProc(hWnd, msg, wParam, lParam);
}