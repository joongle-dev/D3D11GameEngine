#include "Framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = 0.0f;
	m_viewport.Height = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

void RenderTarget::Create(const UINT & width, const UINT & height, const DXGI_FORMAT & pixel, const DXGI_FORMAT & depth)
{
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(height);
	m_graphics->CreateRenderTargetView(width, height, m_rendertarget.ReleaseAndGetAddressOf(), m_shaderresource.ReleaseAndGetAddressOf());
	m_graphics->CreateDepthStencilView(width, height, m_depthstencil.ReleaseAndGetAddressOf());
}

void RenderTarget::Bind()
{
	//Bind viewport, render target view and depth stencil view to pipeline
	m_graphics->GetDeviceContext()->RSSetViewports(1, &m_viewport);
	m_graphics->GetDeviceContext()->OMSetRenderTargets(1, m_rendertarget.GetAddressOf(), m_depthstencil.Get());
}

void RenderTarget::Clear(const Color & color)
{
	m_graphics->GetDeviceContext()->ClearRenderTargetView(m_rendertarget.Get(), color);
	m_graphics->GetDeviceContext()->ClearDepthStencilView(m_depthstencil.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
