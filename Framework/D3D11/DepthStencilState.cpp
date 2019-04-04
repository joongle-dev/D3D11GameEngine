#include "Framework.h"
#include "DepthStencilState.h"


DepthStencilState::DepthStencilState(Context* context)
{
	m_graphics = context->GetSubsystem<Graphics>();

	ZeroMemory(&m_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	m_desc.DepthEnable = true;
	m_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_desc.StencilEnable = true;
	m_desc.StencilReadMask = 0xFF;
	m_desc.StencilWriteMask = 0xFF;
	m_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	m_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	m_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void DepthStencilState::Create()
{
	HRESULT hr = m_graphics->GetDevice()->CreateDepthStencilState(&m_desc, m_state.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void DepthStencilState::Bind()
{
	m_graphics->GetDeviceContext()->OMSetDepthStencilState(m_state.Get(), 0);
}
