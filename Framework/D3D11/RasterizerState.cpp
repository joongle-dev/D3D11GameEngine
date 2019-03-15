#include "Framework.h"
#include "RasterizerState.h"


RasterizerState::RasterizerState(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();

	ZeroMemory(&m_desc, sizeof(D3D11_RASTERIZER_DESC));
	m_desc.AntialiasedLineEnable = true;
	m_desc.CullMode = D3D11_CULL_NONE;
	m_desc.DepthBias = 0;
	m_desc.DepthBiasClamp = 0.0f;
	m_desc.DepthClipEnable = true;
	m_desc.FillMode = D3D11_FILL_SOLID;
	m_desc.FrontCounterClockwise = false;
	m_desc.MultisampleEnable = false;
	m_desc.ScissorEnable = false;
	m_desc.SlopeScaledDepthBias = 0.0f;
}

void RasterizerState::Create()
{
	HRESULT hr = m_graphics->GetDevice()->CreateRasterizerState(&m_desc, m_state.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void RasterizerState::BindPipeline()
{
	m_graphics->GetDeviceContext()->RSSetState(m_state.Get());
}