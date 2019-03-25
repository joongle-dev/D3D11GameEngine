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
	UINT width = window->GetWidth();
	UINT height = window->GetHeight();

	//Create DirectX interface factory
	ComPtr<IDXGIFactory> pFactory;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());
	assert((SUCCEEDED(hr), "Failed to initialize DXGI"));

	//Use the factory to create an adapter for the primary graphics interface (video card).
	ComPtr<IDXGIAdapter> pAdapter;
	hr = pFactory->EnumAdapters(0, pAdapter.GetAddressOf());
	assert((SUCCEEDED(hr), "Failed to initialize DXGI"));

	//Enumerate the primary adapter output (monitor).
	ComPtr<IDXGIOutput> pAdapterOutput;
	hr = pAdapter->EnumOutputs(0, pAdapterOutput.GetAddressOf());
	assert((SUCCEEDED(hr), "Failed to initialize DXGI"));

	//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	UINT nModes;
	hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, NULL);
	assert((SUCCEEDED(hr)));

	//Create a list to hold all the possible display modes for this monitor/video card combination.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[nModes];
	hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, pDisplayModeList);
	assert((SUCCEEDED(hr)));

	//Now go through all the display modes and find the one that matches the screen width and height.
	for (UINT i = 0; i < nModes; i++)
	{
		if (pDisplayModeList[i].Width == width && pDisplayModeList[i].Height == height)
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
	assert((SUCCEEDED(hr)));
	m_gpuMemorySize = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	m_gpuDescription = adapterDesc.Description;

	//Create swap chain, render target view, and depth stencil view
	CreateSwapChain(window->GetHandle());
	SetResolution(width, height);

	window->AttachResizeProc([this](const UINT& width, const UINT& height){ SetResolution(width, height); });
}

void Graphics::SetViewport(const UINT& width, const UINT& height)
{
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
}

void Graphics::SetResolution(const UINT & width, const UINT & height)
{

	m_depthstencilview.ReleaseAndGetAddressOf();
	m_rendertargetview.ReleaseAndGetAddressOf();

	auto hr = m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, NULL);
	assert((SUCCEEDED(hr)));

	CreateRenderTargetView();
	CreateDepthStencilView(width, height, m_depthstencilview.ReleaseAndGetAddressOf());
	SetViewport(width, height);
}

void Graphics::Begin()
{
	//Set the viewport
	m_devicecontext->RSSetViewports(1, &m_viewport);
	//Set the render target view
	m_devicecontext->OMSetRenderTargets(1, m_rendertargetview.GetAddressOf(), m_depthstencilview.Get());
	//Clear the back buffer
	m_devicecontext->ClearRenderTargetView(m_rendertargetview.Get(), m_color);
	//Clear the depth buffer
	m_devicecontext->ClearDepthStencilView(m_depthstencilview.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::End()
{
	//Present the back buffer
	if (m_vsync)
		m_swapchain->Present(1, 0);
	else
		m_swapchain->Present(0, 0);
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
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]),
		D3D11_SDK_VERSION, &swapChainDesc, m_swapchain.GetAddressOf(), m_device.GetAddressOf(), NULL, m_devicecontext.GetAddressOf());
	assert((SUCCEEDED(hr)));
}

void Graphics::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> pBackBuffer;

	//Get the pointer to the back buffer.
	HRESULT hr = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());
	assert((SUCCEEDED(hr)));

	//Create the render target view with the back buffer pointer.
	hr = m_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_rendertargetview.GetAddressOf());
	assert((SUCCEEDED(hr)));
}

void Graphics::CreateRenderTargetView(const UINT & width, const UINT & height, ID3D11RenderTargetView ** rtv, ID3D11ShaderResourceView ** srv, const DXGI_FORMAT & format)
{
	ComPtr<ID3D11Texture2D> texture;

	//Create 2D texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		auto hr = m_device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());
		assert((SUCCEEDED(hr)));
	}

	//Create render target view
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		auto hr = m_device->CreateRenderTargetView(texture.Get(), &desc, rtv);
		assert((SUCCEEDED(hr)));
	}

	//Create shader resource view
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.MostDetailedMip = 0;

		auto hr = m_device->CreateShaderResourceView(texture.Get(), &desc, srv);
		assert((SUCCEEDED(hr)));
	}
}

void Graphics::CreateDepthStencilView(const UINT & width, const UINT & height, ID3D11DepthStencilView ** view, const DXGI_FORMAT & format)
{
	ComPtr<ID3D11Texture2D> buffer;

	//Create 2D texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		auto hr = m_device->CreateTexture2D(&desc, NULL, buffer.GetAddressOf());
		assert((SUCCEEDED(hr)));
	}

	//Create depth stencil view
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		auto hr = m_device->CreateDepthStencilView(buffer.Get(), &desc, view);
		assert(SUCCEEDED(hr));
	}
}

void Graphics::CreateBuffer(ID3D11Buffer ** buffer, const D3D11_BIND_FLAG & bindflag, const UINT & bytewidth, const void * initialdata, const D3D11_USAGE & usage)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = bindflag;
	desc.ByteWidth = bytewidth;
	desc.Usage = usage;
	desc.CPUAccessFlags = usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	if (initialdata) 
	{
		D3D11_SUBRESOURCE_DATA subresource;
		ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
		subresource.pSysMem = initialdata;

		m_device->CreateBuffer(&desc, &subresource, buffer);
	}
	else 
	{
		m_device->CreateBuffer(&desc, nullptr, buffer);
	}
}
