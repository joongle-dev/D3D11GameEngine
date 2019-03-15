#include "Framework.h"
#include "Graphics.h"


template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Graphics::Graphics(Context* context) :
	Subsystem<Graphics>(context),
	m_vsync(false),
	m_fullscreen(false),
	m_color{ 0, 0, 0, 0 }
{
	Window* window = m_context->GetSubsystem<Window>();
	m_width = window->GetWidth();
	m_height = window->GetHeight();

	//Create DirectX interface factory
	ComPtr<IDXGIFactory> pFactory;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());
	assert(SUCCEEDED(hr), "Failed to initialize DXGI");

	//Use the factory to create an adapter for the primary graphics interface (video card).
	ComPtr<IDXGIAdapter> pAdapter;
	hr = pFactory->EnumAdapters(0, pAdapter.GetAddressOf());
	assert(SUCCEEDED(hr), "Failed to initialize DXGI");

	//Enumerate the primary adapter output (monitor).
	ComPtr<IDXGIOutput> pAdapterOutput;
	hr = pAdapter->EnumOutputs(0, pAdapterOutput.GetAddressOf());
	assert(SUCCEEDED(hr), "Failed to initialize DXGI");

	//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	UINT nModes;
	hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, NULL);
	assert(SUCCEEDED(hr));

	//Create a list to hold all the possible display modes for this monitor/video card combination.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[nModes];
	hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, pDisplayModeList);
	assert(SUCCEEDED(hr));

	//Now go through all the display modes and find the one that matches the screen width and height.
	for (int i = 0; i < nModes; i++)
	{
		if (pDisplayModeList[i].Width == m_width && pDisplayModeList[i].Height == m_height)
		{
			//When a match is found store the numerator and denominator of the refresh rate for that monitor.
			m_numerator = pDisplayModeList[i].RefreshRate.Numerator;
			m_denominator = pDisplayModeList[i].RefreshRate.Denominator;
		}
	}
	delete[] pDisplayModeList;
	pDisplayModeList = nullptr;


	//Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	hr = pAdapter->GetDesc(&adapterDesc);
	assert(SUCCEEDED(hr));
	m_gpuMemorySize = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	m_gpuDescription = adapterDesc.Description;

	//Create swap chain, render target view, and depth stencil view
	CreateSwapChain(window->GetHandle());
	SetResolution(m_width, m_height);
}

void Graphics::SetViewport(const unsigned int& width, const unsigned int& height)
{
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
}

void Graphics::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	m_color[0] = r;
	m_color[1] = g;
	m_color[2] = b;
	m_color[3] = a;
}

void Graphics::SetResolution(const unsigned int & width, const unsigned int & height)
{
	m_width = width;
	m_height = height;

	HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, NULL);
	assert(SUCCEEDED(hr));

	CreateRenderTargetView();
	CreateDepthStencilView(width, height, m_depthStencilView.GetAddressOf());
	SetViewport(width, height);
}

void Graphics::Begin()
{
	//Set the viewport
	m_deviceContext->RSSetViewports(1, &m_viewport);
	//Set the render target view
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	//Clear the back buffer
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), m_color);
	//Clear the depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::End()
{
	//Present the back buffer
	if (m_vsync)
		m_swapChain->Present(1, 0);
	else
		m_swapChain->Present(0, 0);
}

void Graphics::CreateSwapChain(HWND hWnd)
{
	//Create the swap chain description.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = m_vsync ? m_numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = m_vsync ? m_denominator : 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !m_fullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	//Create the swap chain, Direct3D device, and Direct3D device context.
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 4,
		D3D11_SDK_VERSION, &swapChainDesc, m_swapChain.GetAddressOf(), m_device.GetAddressOf(), NULL, m_deviceContext.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Graphics::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> pBackBuffer;

	//Get the pointer to the back buffer.
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	//Create the render target view with the back buffer pointer.
	hr = m_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_renderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Graphics::CreateRenderTargetView(const UINT & width, const UINT & height, ID3D11RenderTargetView ** rtv, ID3D11ShaderResourceView ** srv, const DXGI_FORMAT & format)
{
}

void Graphics::CreateDepthStencilView(const UINT & width, const UINT & height, ID3D11DepthStencilView ** view, const DXGI_FORMAT & format)
{
	//Set up the description of the depth buffer.
	D3D11_TEXTURE2D_DESC bufferdesc;
	ZeroMemory(&bufferdesc, sizeof(bufferdesc));
	bufferdesc.Width = width;
	bufferdesc.Height = height;
	bufferdesc.MipLevels = 1;
	bufferdesc.ArraySize = 1;
	bufferdesc.Format = format;
	bufferdesc.SampleDesc.Count = 1;
	bufferdesc.SampleDesc.Quality = 0;
	bufferdesc.Usage = D3D11_USAGE_DEFAULT;
	bufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	bufferdesc.CPUAccessFlags = 0;
	bufferdesc.MiscFlags = 0;

	//Create the texture for the depth buffer using the filled out description.
	ComPtr<ID3D11Texture2D> buffer;
	HRESULT hr = m_device->CreateTexture2D(&bufferdesc, NULL, buffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	//Set up the depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC viewdesc;
	ZeroMemory(&viewdesc, sizeof(viewdesc));
	viewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	viewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewdesc.Texture2D.MipSlice = 0;

	//Create the depth stencil view.
	hr = m_device->CreateDepthStencilView(buffer.Get(), &viewdesc, view);
	assert(SUCCEEDED(hr));
}
