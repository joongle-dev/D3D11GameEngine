#include "Framework.h"
#include "BlendState.h"


BlendState::BlendState(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();

	ZeroMemory(&m_desc, sizeof(D3D11_BLEND_DESC));
	m_desc.RenderTarget[0].BlendEnable = true;
	m_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	m_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	m_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	m_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void BlendState::Create()
{
	HRESULT hr = m_graphics->GetDevice()->CreateBlendState(&m_desc, m_state.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void BlendState::Bind()
{
	m_graphics->GetDeviceContext()->OMSetBlendState(m_state.Get(), 0, 0xffffffff);
}
