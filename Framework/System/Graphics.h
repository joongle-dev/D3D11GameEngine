#pragma once
#include "Subsystem.h"

class Graphics final : public Subsystem<Graphics>
{
public:
	Graphics(class Context* context);
	~Graphics() = default;

	void SetClearColor(const float& r, const float& g, const float& b, const float& a);
	void SetResolution(const unsigned int& width, const unsigned int& height);

	void Begin();
	void End();

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext.Get(); }

	const float GetAspectRatio() const { return (float)m_width / (float)m_height; }

private:
	void CreateSwapChain(HWND hWnd);
	void CreateRenderTargetView();
	void CreateDepthStencilView(const unsigned int& width, const unsigned int& height);
	void SetViewport(const unsigned int& width, const unsigned int& height);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	D3D11_VIEWPORT m_viewport;
	unsigned int m_width;
	unsigned int m_height;
	float m_color[4];

	bool m_fullscreen;
	bool m_vsync;
	
	//Video adapter description
	std::wstring m_gpuDescription;
	unsigned int m_gpuMemorySize;
	unsigned int m_numerator;
	unsigned int m_denominator;
};