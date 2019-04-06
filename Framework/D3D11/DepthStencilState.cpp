#include "Framework.h"
#include "DepthStencilState.h"


DepthStencilState::DepthStencilState(Context* context)
{
	mGraphics = context->GetSubsystem<Graphics>();

	ZeroMemory(&mDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	mDepthStencilDesc.DepthEnable = true;
	mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mDepthStencilDesc.StencilEnable = true;
	mDepthStencilDesc.StencilReadMask = 0xFF;
	mDepthStencilDesc.StencilWriteMask = 0xFF;
	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void DepthStencilState::Create()
{
	HRESULT hr = mGraphics->GetDevice()->CreateDepthStencilState(&mDepthStencilDesc, mDepthStencilState.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void DepthStencilState::Bind()
{
	mGraphics->GetDeviceContext()->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
}
