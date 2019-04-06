#include "Framework.h"
#include "BlendState.h"


BlendState::BlendState(Context * context)
{
	mGraphics = context->GetSubsystem<Graphics>();

	ZeroMemory(&mBlendDesc, sizeof(D3D11_BLEND_DESC));
	mBlendDesc.RenderTarget[0].BlendEnable = true;
	mBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	mBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	mBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	mBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	mBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void BlendState::Create()
{
	HRESULT hr = mGraphics->GetDevice()->CreateBlendState(&mBlendDesc, mBlendState.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void BlendState::Bind()
{
	mGraphics->GetDeviceContext()->OMSetBlendState(mBlendState.Get(), 0, 0xffffffff);
}
