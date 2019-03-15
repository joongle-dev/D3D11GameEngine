#pragma once
#include "Subsystem.h"

class Graphics final : public Subsystem<Graphics>
{
public:
	Graphics(class Context* context);
	~Graphics() = default;

	void Begin();
	void End();

	ID3D11Device* GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() const { return m_devicecontext.Get(); }

	void SetClearColor(const Color& color) { m_color = color; }
	void SetResolution(const UINT& width, const UINT& height);

	const float GetAspectRatio() const { return m_viewport.Width / m_viewport.Height; }

public:
	void CreateRenderTargetView(
		const UINT& width,
		const UINT& height,
		ID3D11RenderTargetView** rtv,
		ID3D11ShaderResourceView** srv = nullptr,
		const DXGI_FORMAT& format = DXGI_FORMAT_R8G8B8A8_UNORM
	);
	void CreateDepthStencilView(
		const UINT& width,
		const UINT& height,
		ID3D11DepthStencilView** dsv,
		const DXGI_FORMAT& format = DXGI_FORMAT_D24_UNORM_S8_UINT
	);

private:
	void CreateSwapChain(HWND hWnd);
	void CreateRenderTargetView();
	void SetViewport(const UINT& width, const UINT& height);

private:
	Microsoft::WRL::ComPtr<ID3D11Device>        m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_devicecontext;

	Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rendertargetview;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthstencilview;

	D3D11_VIEWPORT m_viewport;
	Color m_color;

	bool m_fullscreen;
	bool m_vsync;
	
	//Video adapter description
	std::wstring m_gpuDescription;
	unsigned int m_gpuMemorySize;
	unsigned int m_numerator;
	unsigned int m_denominator;
};